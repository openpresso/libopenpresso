#ifndef SENSORS_DEBOUNCED_PIN_STATE_DEBOUNCED_PIN_STATE_HPP
#define SENSORS_DEBOUNCED_PIN_STATE_DEBOUNCED_PIN_STATE_HPP

#include <atomic>
#include <memory>
#include <optional>
#include <unordered_map>

#include <libopenpresso/interfaces/logical_input.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <gpio/pin_monitor.hpp>
#include <utils/on_timeout.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso
{

class SwDebouncedButtonState final : public interfaces::LogicalInput {
public:
  SwDebouncedButtonState(const std::shared_ptr<gpio::PinMonitor>& monitor, time_delta_t debounce, bool inverted);

  bool getState() const override;
  std::optional<time_delta_t> fixedUpdateRate() const noexcept override;
  callback_descriptor_t registerCallback(const callback_t& callback) override;
  void unregisterCallback(callback_descriptor_t descr) override;

private:
  void noDebounceCallback(gpio::PinEvent event);
  void debounceCallback(gpio::PinEvent event);

private:
  std::atomic<bool> m_isPressed = false;
  const bool m_inverted = false;
  std::optional<TaskOnTimeout> m_delayedCallback;
  time_delta_t m_timeout;

  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, callback_t> m_callbacks;

  gpio::PinMonitorCallbackHandler m_monitorCallback;
};

} // namespace libopenpresso

#endif // SENSORS_DEBOUNCED_PIN_STATE_DEBOUNCED_PIN_STATE_HPP