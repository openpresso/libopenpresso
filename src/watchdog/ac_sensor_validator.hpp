#ifndef WATCHDOG_AC_SENSOR_VALIDATOR_HPP
#define WATCHDOG_AC_SENSOR_VALIDATOR_HPP

#include <atomic>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>

#include <libopenpresso/types.hpp>

#include <gpio/pin_monitor.hpp>

namespace libopenpresso::watchdog
{
class WatchdogThread;

using namespace std::chrono_literals;

class AcSensorValidator {
  using FloatMillis = std::chrono::duration<float, std::chrono::milliseconds::period>;

  static constexpr auto UPDATE_INTERVAL = 30ms;

public:
  AcSensorValidator(const std::shared_ptr<WatchdogThread>& watchdog,
                    const std::shared_ptr<gpio::PinMonitor>& monitor);
  AcSensorValidator(const AcSensorValidator&) = delete;
  AcSensorValidator(AcSensorValidator&&) = delete;
  auto operator=(const AcSensorValidator&) = delete;
  auto operator=(AcSensorValidator&&) = delete;
  ~AcSensorValidator();

  bool operator()();

  auto makePinEventCallback();

private:
  std::optional<std::chrono::steady_clock::time_point> m_callbackCountFirstCheck;
  std::atomic<size_t> m_risingCounter = 0;
  std::atomic<size_t> m_fallingCounter = 0;
  gpio::PinMonitorCallbackHandler m_callback;
  std::shared_ptr<WatchdogThread> m_watchdog;
  const float m_updateRatio;
  const size_t m_callbacksExpected;
  const size_t m_callbacksCheckFrequency;
  size_t m_callbacksCheckCounter = 0;
  callback_descriptor_t m_validatorDescriptor;
};

} // namespace libopenpresso::watchdog

#endif // WATCHDOG_AC_SENSOR_VALIDATOR_HPP