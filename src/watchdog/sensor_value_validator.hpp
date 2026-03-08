#ifndef WATCHDOG_SENSOR_VALUE_VALIDATOR_HPP
#define WATCHDOG_SENSOR_VALUE_VALIDATOR_HPP

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>

#include <libopenpresso/types.hpp>

#include <utils/callback_descriptor_handler.hpp>
#include <watchdog/watchdog_thread.hpp>

namespace libopenpresso::watchdog
{

class SensorValueObserver {
public:
  template <typename ValueT>
  SensorValueObserver(const auto& sensor, ValueT minValue, ValueT maxValue)
  : m_callbackDescriptor{sensor, sensor->registerCallback(makeCallback(minValue, maxValue))}
  {
  }
  size_t resetCallbacksCount() noexcept;
  bool isWithinLimits() const noexcept;

private:
  template <typename ValueT>
  auto makeCallback(ValueT minValue, ValueT maxValue)
  {
    return [this, minValue, maxValue](ValueT value) {
      if (value < minValue || maxValue < value) {
        m_withinLimits.store(false, std::memory_order_relaxed);
      }
      m_callbacksCounter.fetch_add(1, std::memory_order_relaxed);
    };
  }

private:
  std::atomic<bool> m_withinLimits = true;
  std::atomic<size_t> m_callbacksCounter = 0;
  CallbackDescriptorHandler m_callbackDescriptor;
};

class SensorValueValidator {
  using FloatMillis = std::chrono::duration<float, std::chrono::milliseconds::period>;

public:
  template <typename ValueT>
  SensorValueValidator(const std::shared_ptr<WatchdogThread>& watchdog,
                       const auto& sensor,
                       ValueT minValue,
                       ValueT maxValue,
                       time_delta_t updateInterval)
  : m_callbackCountFirstCheck{std::chrono::steady_clock::now() + 2 * updateInterval}
  , m_watchdog{watchdog}
  , m_observer{sensor, minValue, maxValue}
  , m_updateRatio{FloatMillis{watchdog->validationInterval()} / FloatMillis{updateInterval}}
  , m_callbacksExpected{std::max<size_t>(1, m_updateRatio / 2)}
  , m_callbacksCheckFrequency{std::max<size_t>(1, std::ceil(2.0f / m_updateRatio))}
  , m_validatorDescriptor{m_watchdog->registerValidator(std::reference_wrapper(*this))}
  {
  }

  SensorValueValidator(const SensorValueValidator&) = delete;
  SensorValueValidator(SensorValueValidator&&) = delete;
  auto operator=(const SensorValueValidator&) = delete;
  auto operator=(SensorValueValidator&&) = delete;

  ~SensorValueValidator();

  bool operator()();

private:
  std::optional<std::chrono::steady_clock::time_point> m_callbackCountFirstCheck;
  std::shared_ptr<WatchdogThread> m_watchdog;
  SensorValueObserver m_observer;
  const float m_updateRatio;
  const size_t m_callbacksExpected;
  const size_t m_callbacksCheckFrequency;
  size_t m_callbacksCheckCounter = 0;

  callback_descriptor_t m_validatorDescriptor;
};

} // namespace libopenpresso::watchdog

#endif // WATCHDOG_SENSOR_VALUE_VALIDATOR_HPP