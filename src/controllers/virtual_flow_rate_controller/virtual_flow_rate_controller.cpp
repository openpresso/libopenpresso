#include "virtual_flow_rate_controller.hpp"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <memory>
#include <ratio>
#include <utility>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pulse_controller.hpp>

using namespace libopenpresso;

VirtualFlowRateController::VirtualFlowRateController(std::shared_ptr<gpio::PulseController> controller,
                                                     PressureSensorPtr sensor,
                                                     millibars_t pumpStallPressure,
                                                     micrograms_t volumePerPulse,
                                                     size_t mainsFrequency)
: m_controller{std::move(controller)}
, m_sensor{std::move(sensor)}
, m_pumpStallPressure{pumpStallPressure}
, m_volumePerPulse{volumePerPulse}
, m_mainsFrequency{mainsFrequency}
{
}

auto VirtualFlowRateController::getPulseCallback() const
{
  using namespace std::chrono_literals;
  return [this, accum = 0u] mutable {
    auto pressureCoef = m_pumpStallPressure - std::min(m_sensor->getPressure(), m_pumpStallPressure);
    auto weightAdded = pressureCoef * m_volumePerPulse / m_pumpStallPressure;
    accum += std::milli::den * m_targetRate.load(std::memory_order_relaxed) / m_mainsFrequency;

    if (accum >= weightAdded) {
      accum -= weightAdded;
      return true;
    }

    return false;
  };
}

void VirtualFlowRateController::activate()
{
  if (isActive()) {
    throw libopenpresso::Exception{"VirtualFlowRateController is already active"};
  }

  m_controller->activate(getPulseCallback());
  m_isActive = true;
}

void VirtualFlowRateController::deactivate()
{
  if (!isActive()) {
    return;
  }

  m_controller->deactivate();
  m_isActive = false;
}

bool VirtualFlowRateController::isActive() const noexcept
{
  return m_isActive;
}

milligrams_p_second_t VirtualFlowRateController::getTargetRate() const
{
  return m_targetRate.load(std::memory_order_relaxed);
}

void VirtualFlowRateController::setTargetRate(milligrams_p_second_t rate)
{
  m_targetRate.store(rate, std::memory_order_relaxed);
}
