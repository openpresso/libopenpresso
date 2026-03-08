#include "sensor_value_validator.hpp"

#include <atomic>
#include <chrono>
#include <cstddef>

using namespace libopenpresso::watchdog;

size_t SensorValueObserver::resetCallbacksCount() noexcept
{
  return m_callbacksCounter.exchange(0, std::memory_order_relaxed);
}

bool SensorValueObserver::isWithinLimits() const noexcept
{
  return m_withinLimits.load(std::memory_order_relaxed);
}

SensorValueValidator::~SensorValueValidator()
{
  m_watchdog->unregisterValidator(m_validatorDescriptor);
}

bool SensorValueValidator::operator()()
{
  if (!m_observer.isWithinLimits()) {
    return false;
  }

  if (m_callbackCountFirstCheck.has_value()) {
    if (std::chrono::steady_clock::now() < m_callbackCountFirstCheck.value()) {
      return true;
    }
    m_callbackCountFirstCheck.reset();
  }

  if (m_callbacksCheckCounter == 0) {
    return m_observer.resetCallbacksCount() >= m_callbacksExpected;
  }
  m_callbacksCheckCounter = (m_callbacksCheckCounter + 1) % m_callbacksCheckFrequency;

  return true;
}
