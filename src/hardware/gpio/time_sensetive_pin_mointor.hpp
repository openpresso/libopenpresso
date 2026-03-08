#ifndef HARDWARE_GPIO_TIME_SENSETIVE_PIN_MOINTOR_HPP
#define HARDWARE_GPIO_TIME_SENSETIVE_PIN_MOINTOR_HPP

#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <gpio/pin_event_handler.hpp>
#include <gpio/pin_monitor.hpp>
#include <utils/epoll_thread.hpp>

namespace libopenpresso::gpio
{

class TimeSensetivePinMonitor : public PinMonitor {
  static constexpr int THREAD_PRIORITY = 80;

public:
  TimeSensetivePinMonitor(const input_pin_info_t& pin);
  TimeSensetivePinMonitor(const TimeSensetivePinMonitor&) = delete;
  TimeSensetivePinMonitor(TimeSensetivePinMonitor&&) = delete;
  auto operator=(const TimeSensetivePinMonitor&) -> TimeSensetivePinMonitor& = delete;
  auto operator=(TimeSensetivePinMonitor&&) -> TimeSensetivePinMonitor& = delete;
  ~TimeSensetivePinMonitor() = default;

  callback_descriptor_t registerCallback(PinEvent notifyOn, const PinEventCallback_t& callback) override;
  void unregisterCallback(callback_descriptor_t callbackDescriptor) override;
  PinState readState() const override;

private:
  PinEventHandler m_eventHandler;
  EpollThread m_thread;
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_TIME_SENSETIVE_PIN_MOINTOR_HPP