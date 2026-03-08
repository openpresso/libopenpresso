#include "pulse_power_controller.hpp"

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pulse_controller.hpp>

using namespace libopenpresso::gpio;

PulsePowerController::PulsePowerController(const std::shared_ptr<PulseController>& pulseController,
                                           uint8_t maxPower)
: m_threshold{maxPower}
, m_pulseController{pulseController}
{
  if (maxPower == 0) {
    throw libopenpresso::Exception{"Max power must be greater than 0, got {}", maxPower};
  }
}

libopenpresso::power_units_t PulsePowerController::powerMax() const noexcept
{
  return m_threshold;
}

void PulsePowerController::activate()
{
  if (isActive()) {
    throw libopenpresso::Exception{"PulsePowerController is already active"};
  }

  m_errorAccum = 0;
  m_pulseController->activate([this] { return zeroCrossCallback(); });
  m_isActive = true;
}

void PulsePowerController::deactivate()
{
  if (!isActive()) {
    return;
  }

  m_pulseController->deactivate();
  m_isActive = false;
}

bool PulsePowerController::isActive() const noexcept
{
  return m_isActive;
}

void PulsePowerController::setTargetPower(power_units_t power) noexcept
{
  m_target.store(power, std::memory_order_relaxed);
}

bool PulsePowerController::zeroCrossCallback()
{
  m_errorAccum += m_target.load(std::memory_order_relaxed);

  if (m_errorAccum >= m_threshold) {
    m_errorAccum -= m_threshold;
    return true;
  }

  return false;
}
