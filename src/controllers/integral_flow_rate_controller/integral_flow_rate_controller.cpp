#include "integral_flow_rate_controller.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <memory>
#include <utility>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/power_controller.hpp>

using namespace libopenpresso;

IntegralFlowRateController::IntegralFlowRateController(
  std::shared_ptr<gpio::PowerController> powerController, WeightSensorPtr sensor, pid_coeffs_t coef)
: m_coef{coef}
, m_control{std::move(powerController)}
, m_sensor{std::move(sensor)}
{
}

auto IntegralFlowRateController::getFixedUpdateRateCallback(time_delta_t updateRate)
{
  return [this, updateRate, accum = 0.0f](milligrams_t weight, milligrams_p_second_t rate) mutable {
    weightSensorCallback(weight, rate, updateRate, accum);
  };
}

auto IntegralFlowRateController::getVariableUpdateRateCallback()
{
  return [this,
          prevTime = std::chrono::steady_clock::now(),
          accum = 0.0f](milligrams_t weight, milligrams_p_second_t rate) mutable {
    auto now = std::chrono::steady_clock::now();
    auto dt = now - std::exchange(prevTime, now);
    weightSensorCallback(weight, rate, dt, accum);
  };
}

void IntegralFlowRateController::activate()
{
  if (isActive()) {
    throw libopenpresso::Exception{"Controller is already active"};
  }

  m_control->setTargetPower(0);
  m_control->activate();

  auto fixedUpdateRate = m_sensor->fixedUpdateRate();
  if (fixedUpdateRate.has_value()) {
    m_cbDescriptor.emplace(
      m_sensor, m_sensor->registerCallback(getFixedUpdateRateCallback(fixedUpdateRate.value())));
  }
  else {
    m_cbDescriptor.emplace(m_sensor, m_sensor->registerCallback(getVariableUpdateRateCallback()));
  }
}

void IntegralFlowRateController::deactivate()
{
  if (!isActive()) {
    return;
  }

  m_cbDescriptor.reset();
  m_control->deactivate();
}

bool IntegralFlowRateController::isActive() const noexcept
{
  return m_cbDescriptor.has_value();
}

milligrams_p_second_t IntegralFlowRateController::getTargetRate() const
{
  return m_targetRate.load(std::memory_order_relaxed);
}

void IntegralFlowRateController::setTargetRate(milligrams_p_second_t rate)
{
  m_targetRate.store(rate, std::memory_order_relaxed);
}

void IntegralFlowRateController::weightSensorCallback([[maybe_unused]] milligrams_t weight,
                                                      milligrams_p_second_t rate,
                                                      std::chrono::duration<float> dt,
                                                      float& accum)
{
  const milligrams_p_second_t targetRate = m_targetRate.load(std::memory_order_relaxed);
  const milligrams_p_second_t error = targetRate - rate;
  accum += m_coef * error * dt.count();
  accum = std::clamp(accum, 0.0f, 1.0f);
  m_control->setTargetPower(accum * m_control->powerMax());
}
