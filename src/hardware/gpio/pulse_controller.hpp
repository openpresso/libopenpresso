#ifndef HARDWARE_GPIO_PULSE_CONTROLLER_HPP
#define HARDWARE_GPIO_PULSE_CONTROLLER_HPP

#include <functional>
#include <memory>
#include <optional>

#include <gpio/pin_monitor.hpp>

namespace libopenpresso::gpio
{

class PinOutput;

class PulseCounter {
public:
  virtual void countPulse() noexcept = 0;
  virtual ~PulseCounter() = default;
};

class PulseController {
public:
  PulseController(const std::shared_ptr<PinMonitor>& zeroCrossMonitor,
                  const std::shared_ptr<PinOutput>& controlPin);

  void setPulseCounter(const std::shared_ptr<PulseCounter>& counter);
  void unsetPulseCounter();

  bool isActive() const noexcept;
  void activate(std::function<bool()> firePulseCallback);
  void deactivate();

private:
  std::shared_ptr<PulseCounter> m_counter;
  std::shared_ptr<PinOutput> m_control;
  std::shared_ptr<PinMonitor> m_zeroCrossMonitor;
  std::optional<PinMonitorCallbackHandler> m_cbDescriptor;
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_PULSE_CONTROLLER_HPP