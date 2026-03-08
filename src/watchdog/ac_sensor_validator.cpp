#include "ac_sensor_validator.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <functional>
#include <memory>

#include <gpio/pin_data.hpp>
#include <gpio/pin_monitor.hpp>
#include <utils/logger.hpp>
#include <watchdog/watchdog_thread.hpp>

using namespace libopenpresso::watchdog;

auto AcSensorValidator::makePinEventCallback()
{
  return [this](gpio::PinEvent event) {
    switch (event) {
    case gpio::PinEvent::RisingEdge:
      m_risingCounter.fetch_add(1, std::memory_order_relaxed);
      break;
    case gpio::PinEvent::FallingEdge:
      m_fallingCounter.fetch_add(1, std::memory_order_relaxed);
      break;
    default:
      break;
    }
  };
}

AcSensorValidator::AcSensorValidator(const std::shared_ptr<WatchdogThread>& watchdog,
                                     const std::shared_ptr<gpio::PinMonitor>& monitor)
: m_callbackCountFirstCheck{std::chrono::steady_clock::now() + 2 * UPDATE_INTERVAL}
, m_callback{monitor, gpio::PinEvent::Both, makePinEventCallback()}
, m_watchdog{watchdog}
, m_updateRatio{FloatMillis{watchdog->validationInterval()} / FloatMillis{UPDATE_INTERVAL}}
, m_callbacksExpected{std::max<size_t>(1, m_updateRatio / 2)}
, m_callbacksCheckFrequency{std::max<size_t>(1, std::ceil(2.0f / m_updateRatio))}
, m_validatorDescriptor{m_watchdog->registerValidator(std::reference_wrapper(*this))}
{
}

AcSensorValidator::~AcSensorValidator()
{
  m_watchdog->unregisterValidator(m_validatorDescriptor);
}

bool AcSensorValidator::operator()()
{
  if (m_callbackCountFirstCheck.has_value()) {
    if (std::chrono::steady_clock::now() < m_callbackCountFirstCheck.value()) {
      return true;
    }
    m_callbackCountFirstCheck.reset();
  }

  if (m_callbacksCheckCounter == 0) {
    if (auto count = m_risingCounter.exchange(0, std::memory_order_relaxed); count < m_callbacksExpected) {
      Logger::err("Rising counter failed, actual count: {}, expected: {}", count, m_callbacksExpected);
      return false;
    }
    if (auto count = m_fallingCounter.exchange(0, std::memory_order_relaxed); count < m_callbacksExpected) {
      Logger::err("Falling counter failed, actual count: {}, expected: {}", count, m_callbacksExpected);
      return false;
    }
    return true;
  }
  m_callbacksCheckCounter = (m_callbacksCheckCounter + 1) % m_callbacksCheckFrequency;

  return true;
}
