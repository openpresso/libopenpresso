#include "pid_temperature_controller.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <memory>
#include <utility>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/pid_settings.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/power_controller.hpp>
#include <pump_flow_sensor/pump_flow_sensor.hpp>
#include <utils/derivative_filter.hpp>

using namespace libopenpresso;

template <bool dumpPidState>
PidTemperatureController<dumpPidState>::PidTemperatureController(
  std::shared_ptr<gpio::PowerController> powerController,
  TemperatureSensorPtr temperatureSensor,
  std::shared_ptr<PumpFlowSensor> flowSensor,
  const PidSettings& pidSettings)
: m_powerController{std::move(powerController)}
, m_temperatureSensor{std::move(temperatureSensor)}
, m_flowSensor{std::move(flowSensor)}
, m_pidSettings{applyUntisMutlipliers(pidSettings)}
{
}

template <bool dumpPidState>
void PidTemperatureController<dumpPidState>::temperatureSensorCallback(millidegrees_t currentTemp,
                                                                       std::chrono::duration<float> dt,
                                                                       auto& dFilter,
                                                                       pid_calc_t& iTerm,
                                                                       pid_calc_t& wTerm)
{
  millidegrees_t targetTemp = m_targetTemp.load(std::memory_order_relaxed);
  micrograms_t weightAdded = m_ffWeight.exchange(0, std::memory_order_relaxed);
  pid_calc_t error = static_cast<pid_calc_t>(targetTemp) - currentTemp;
  pid_calc_t errorXdt = error * dt.count();
  pid_calc_t pTerm = m_pidSettings.p * error;
  pid_calc_t dTerm = -getRelaxedDcoef(error) * dFilter(currentTemp, dt);
  pid_calc_t fTerm = m_pidSettings.f * weightAdded / dt.count();
  pid_calc_t pidSum = pTerm + dTerm + iTerm + fTerm + wTerm;
  pid_calc_t iTermAddition = getRelaxedIcoef(pidSum) * errorXdt;

  if (iTerm >= 0.0f || iTermAddition >= 0.0f) {
    iTerm += iTermAddition;
  }

  wTerm += m_pidSettings.w * weightAdded;
  if (wTerm > 0.0 && error < 0) {
    wTerm -= std::min(wTerm, -m_pidSettings.wDecay * errorXdt);
  }

  pid_calc_t pidOutput = std::clamp(pidSum, 0.0f, PID_RESULT_MAX);
  power_units_t result = pidOutput * m_powerController->powerMax();
  m_powerController->setTargetPower(result);
  if constexpr (dumpPidState) {
    PidStateDump::store(pTerm, iTerm, dTerm, fTerm + wTerm, pidSum);
  }
}

template <bool dumpPidState>
auto PidTemperatureController<dumpPidState>::makeFixedUpdateRateCallback(std::chrono::duration<float> dt)
{
  std::chrono::duration<float> filterTime = m_pidSettings.dFilterTime;
  auto dFilter = [dFilter = FilteredDerivative<float, true>(
                    filterTime.count(), dt.count(), m_temperatureSensor->getTemperature())](
                   millidegrees_t temperature, std::chrono::duration<float>) mutable {
    return dFilter.process(temperature);
  };

  return [this, dt, dFilter = std::move(dFilter), iTerm = pid_calc_t{0}, wTerm = pid_calc_t{0}](
           millidegrees_t currentTemp) mutable {
    temperatureSensorCallback(currentTemp, dt, dFilter, iTerm, wTerm);
  };
}

template <bool dumpPidState>
auto PidTemperatureController<dumpPidState>::makeVariableUpdateRateCallback()
{
  std::chrono::duration<float> filterTime = m_pidSettings.dFilterTime;
  auto dFilter = [dFilter = FilteredDerivative<float, false>(filterTime.count(),
                                                             m_temperatureSensor->getTemperature())](
                   millidegrees_t temperature, std::chrono::duration<float> dt) mutable {
    return dFilter.process(temperature, dt.count());
  };

  return [this,
          dFilter = std::move(dFilter),
          iTerm = pid_calc_t{0},
          wTerm = pid_calc_t{0},
          prevTime = std::chrono::steady_clock::now()](millidegrees_t currentTemp) mutable {
    auto now = std::chrono::steady_clock::now();
    temperatureSensorCallback(currentTemp, now - std::exchange(prevTime, now), dFilter, iTerm, wTerm);
  };
}

template <bool dumpPidState>
void PidTemperatureController<dumpPidState>::activate()
{
  if (isActive()) {
    throw libopenpresso::Exception{"Temperature controller is already active"};
  }

  m_powerController->setTargetPower(0.0f);
  m_powerController->activate();

  auto fixedUpdateRate = m_temperatureSensor->fixedUpdateRate();
  if (fixedUpdateRate.has_value()) {
    m_tempSensorCbDescr.emplace(
      m_temperatureSensor,
      m_temperatureSensor->registerCallback(makeFixedUpdateRateCallback(fixedUpdateRate.value())));
  }
  else {
    m_tempSensorCbDescr.emplace(m_temperatureSensor,
                                m_temperatureSensor->registerCallback(makeVariableUpdateRateCallback()));
  }

  if (m_flowSensor) {
    m_flowSensorCbDescr.emplace(m_flowSensor, m_flowSensor->registerCallback([this](micrograms_t mass) {
      flowCallback(mass);
    }));
  }
}

template <bool dumpPidState>
void PidTemperatureController<dumpPidState>::deactivate()
{
  if (!isActive()) {
    return;
  }

  m_powerController->deactivate();

  m_tempSensorCbDescr.reset();
  m_flowSensorCbDescr.reset();
  if constexpr (dumpPidState) {
    PidStateDump::reset();
  }
}

template <bool dumpPidState>
bool PidTemperatureController<dumpPidState>::isActive() const noexcept
{
  return m_tempSensorCbDescr.has_value();
}

template <bool dumpPidState>
millidegrees_t PidTemperatureController<dumpPidState>::getTargetTemperature() const
{
  return m_targetTemp.load(std::memory_order_relaxed);
}

template <bool dumpPidState>
void PidTemperatureController<dumpPidState>::setTargetTemperature(millidegrees_t millidegrees)
{
  m_targetTemp.store(millidegrees, std::memory_order_relaxed);
}

template <bool dumpPidState>
void PidTemperatureController<dumpPidState>::flowCallback(micrograms_t weight)
{
  m_ffWeight.fetch_add(weight, std::memory_order_relaxed);
}

template <bool dumpPidState>
pid_calc_t PidTemperatureController<dumpPidState>::getRelaxedDcoef(pid_calc_t error)
{
  return std::max(1.0f - m_pidSettings.dTermRelax * std::abs(error), 0.f) * m_pidSettings.d;
}

template <bool dumpPidState>
pid_calc_t PidTemperatureController<dumpPidState>::getRelaxedIcoef(pid_calc_t pidSum)
{
  return std::max(1.0f - std::abs(pidSum) * m_pidSettings.iTermRelax, 0.f) * m_pidSettings.i;
}

template <bool dumpPidState>
PidSettings PidTemperatureController<dumpPidState>::applyUntisMutlipliers(PidSettings settings) noexcept
{
  // NOLINTBEGIN(readability-magic-numbers)
  settings.p *= 1e-3;
  settings.d *= 1e-3;
  settings.dTermRelax *= 1e-3;
  settings.i *= 1e-3;
  settings.w *= 1e-6;
  settings.f *= 1e-6;
  settings.wDecay *= 1e-3;
  return settings;
  // NOLINTEND(readability-magic-numbers)
}

namespace libopenpresso
{

template class PidTemperatureController<true>;
template class PidTemperatureController<false>;

} // namespace libopenpresso