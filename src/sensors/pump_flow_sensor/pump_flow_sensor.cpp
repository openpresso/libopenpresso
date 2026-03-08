#include "pump_flow_sensor.hpp"

#include <algorithm>
#include <atomic>
#include <functional>
#include <mutex>
#include <optional>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/types.hpp>

using namespace libopenpresso;

PumpFlowSensor::PumpFlowSensor(const PressureSensorPtr& pressure,
                               millibars_t pumpStallPressure,
                               micrograms_t volumePerPulse)
: m_pumpStallPressure{pumpStallPressure}
, m_valuePerPulse{volumePerPulse}
, m_pressureSensor{pressure}
, m_pressureCallback{pressure,
                     pressure->registerCallback([this](millibars_t prs) { pressureCallback(prs); })}
{
}

std::optional<time_delta_t> PumpFlowSensor::fixedUpdateRate() const noexcept
{
  return m_pressureSensor->fixedUpdateRate();
}

callback_descriptor_t PumpFlowSensor::registerCallback(const callback_t& callback)
{
  if (!callback) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks.emplace(m_nextCallbackDescriptor, callback);
  return m_nextCallbackDescriptor++;
}

void PumpFlowSensor::unregisterCallback(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_callbacksLock);
  if (auto it = m_callbacks.find(descr); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

void PumpFlowSensor::countPulse() noexcept
{
  m_pulses.fetch_add(1, std::memory_order_relaxed);
}

void PumpFlowSensor::pressureCallback(millibars_t pressure)
{
  auto pulses = m_pulses.exchange(0, std::memory_order_relaxed);
  auto pressureCoef = 1.0f - std::min<float>(pressure, m_pumpStallPressure) / m_pumpStallPressure;
  auto weightAdded = pressureCoef * m_valuePerPulse * pulses;

  std::scoped_lock lock(m_callbacksLock);
  for (auto&& cb : m_callbacks) {
    std::invoke(cb.second, weightAdded);
  }
}
