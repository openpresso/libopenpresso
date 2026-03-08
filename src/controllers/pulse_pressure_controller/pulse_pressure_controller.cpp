#include "pulse_pressure_controller.hpp"

#include <atomic>
#include <cstdlib>
#include <memory>
#include <utility>

#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pulse_controller.hpp>

using namespace libopenpresso;

PulsePressureController::PulsePressureController(std::shared_ptr<gpio::PulseController> controller,
                                                 PressureSensorPtr sensor)
: m_control{std::move(controller)}
, m_sensor{std::move(sensor)}
{
}

PulsePressureController::~PulsePressureController()
{
  try {
    if (m_isActive) {
      m_control->deactivate();
    }
  }
  catch (...) {
    std::abort();
  }
}

void PulsePressureController::activate()
{
  m_control->activate([this] {
    return m_targetPressure.load(std::memory_order_relaxed) > m_sensor->getPressure();
  });
  m_isActive = true;
}

void PulsePressureController::deactivate()
{
  if (!isActive()) {
    return;
  }

  m_control->deactivate();
  m_isActive = false;
}

bool PulsePressureController::isActive() const noexcept
{
  return m_isActive;
}

millibars_t PulsePressureController::getTargetPressure() const
{
  return m_targetPressure.load(std::memory_order_relaxed);
}

void PulsePressureController::setTargetPressure(millibars_t pressure)
{
  m_targetPressure.store(pressure, std::memory_order_relaxed);
}
