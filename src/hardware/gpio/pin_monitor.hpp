#ifndef HARDWARE_GPIO_PIN_MONITOR_HPP
#define HARDWARE_GPIO_PIN_MONITOR_HPP

#include <functional>
#include <memory>

#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <utils/callback_descriptor_handler.hpp>

namespace libopenpresso::gpio
{

class PinMonitor {
public:
  using PinEventCallback_t = std::function<void(PinEvent)>;

  virtual callback_descriptor_t registerCallback(PinEvent notifyOn, const PinEventCallback_t& callback) = 0;
  virtual void unregisterCallback(callback_descriptor_t callbackDescriptor) = 0;
  virtual PinState readState() const = 0;
  virtual ~PinMonitor() = default;
};

class PinMonitorCallbackHandler : private CallbackDescriptorHandler {
public:
  PinMonitorCallbackHandler(const std::shared_ptr<PinMonitor>& monitor,
                            PinEvent notifyOn,
                            const PinMonitor::PinEventCallback_t& callback)
  : CallbackDescriptorHandler{monitor, monitor->registerCallback(notifyOn, callback)}
  {
  }
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_PIN_MONITOR_HPP