#ifndef HARDWARE_GPIO_PIN_EVENT_HANDLER_HPP
#define HARDWARE_GPIO_PIN_EVENT_HANDLER_HPP

#include <unordered_map>
#include <utility>

#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <gpio/pin_monitor.hpp>
#include <utils/fd_wrapper.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso::gpio
{

class PinEventHandler : public PinMonitor {
public:
  PinEventHandler(input_pin_info_t pinInfo);
  bool isEventSupported(PinEvent notifyOn) const noexcept;
  int getFd() const noexcept;
  void processEvent();

  // PinMonitor interface
  callback_descriptor_t registerCallback(PinEvent notifyOn, const PinEventCallback_t& callback) override;
  void unregisterCallback(callback_descriptor_t callbackDescriptor) override;
  PinState readState() const override;

private:
  fd_wrapper makeEventFd() const;

private:
  const input_pin_info_t m_info;
  fd_wrapper m_fd;
  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, std::pair<PinEvent, PinEventCallback_t>> m_callbacks;
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_PIN_EVENT_HANDLER_HPP