#include "pin_event_handler.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <mutex>
#include <type_traits>
#include <unistd.h>
#include <utility>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/pin_info.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <linux/gpio.h>
#include <sys/ioctl.h>

using namespace libopenpresso::gpio;

PinEventHandler::PinEventHandler(input_pin_info_t pinInfo)
: m_info{std::move(pinInfo)}
, m_fd{makeEventFd()}
{
}

bool PinEventHandler::isEventSupported(PinEvent notifyOn) const noexcept
{
  return (static_cast<std::underlying_type_t<PinEvent>>(notifyOn) &
          static_cast<std::underlying_type_t<PinEvent>>(m_info.listenEvents)) ==
         static_cast<std::underlying_type_t<PinEvent>>(notifyOn);
}

int PinEventHandler::getFd() const noexcept
{
  return m_fd.get();
}

void PinEventHandler::processEvent()
{
  gpio_v2_line_event eventInfo{};
  if (read(m_fd.get(), &eventInfo, sizeof(eventInfo)) != sizeof(eventInfo)) {
    throw libopenpresso::SystemError{"Failed to get gpio event info"};
  }

  std::scoped_lock lock(m_callbacksLock);

  for (auto&& [d, cbData] : m_callbacks) {
    auto& [notifyOn, callback] = cbData;
    switch (eventInfo.id) {
    case GPIO_V2_LINE_EVENT_RISING_EDGE:
      if (static_cast<bool>(std::to_underlying(notifyOn) & std::to_underlying(PinEvent::RisingEdge))) {
        std::invoke(callback, PinEvent::RisingEdge);
      }
      break;
    case GPIO_V2_LINE_EVENT_FALLING_EDGE:
      if (static_cast<bool>(std::to_underlying(notifyOn) & std::to_underlying(PinEvent::FallingEdge))) {
        std::invoke(callback, PinEvent::FallingEdge);
      }
      break;
    default:
      break;
    }
  }
}

libopenpresso::callback_descriptor_t PinEventHandler::registerCallback(PinEvent notifyOn,
                                                                       const PinEventCallback_t& callback)
{
  if (!isEventSupported(notifyOn)) {
    throw libopenpresso::Exception{"Pin is not configured for requested event"};
  }

  if (!callback) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks[m_nextCallbackDescriptor] = {notifyOn, callback};
  return m_nextCallbackDescriptor++;
}

void PinEventHandler::unregisterCallback(callback_descriptor_t callbackDescriptor)
{
  std::scoped_lock lock(m_callbacksLock);
  if (auto it = m_callbacks.find(callbackDescriptor); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

PinState PinEventHandler::readState() const
{
  gpio_v2_line_values vals{};
  vals.mask = 1;
  int res = ioctl(m_fd.get(), GPIO_V2_LINE_GET_VALUES_IOCTL, &vals);
  if (res < 0) {
    throw libopenpresso::SystemError{"Failed to get gpio input value"};
  }

  return vals.bits == vals.mask ? PinState::High : PinState::Low;
}

fd_wrapper PinEventHandler::makeEventFd() const
{
  gpio_v2_line_request lineRequest{};
  fd_wrapper chipFd(open(m_info.pinAddr.chip.c_str(), O_RDONLY));
  lineRequest.num_lines = 1;
  lineRequest.offsets[0] = m_info.pinAddr.pin;
  lineRequest.config.flags = GPIO_V2_LINE_FLAG_INPUT;

  switch (m_info.pinPull) {
  case PinPull::No:
    lineRequest.config.flags |= GPIO_V2_LINE_FLAG_BIAS_DISABLED;
    break;
  case PinPull::PullDown:
    lineRequest.config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
    break;
  case PinPull::PullUp:
    lineRequest.config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_UP;
    break;
  default:
    break;
  }

  if (static_cast<bool>(std::to_underlying(m_info.listenEvents) & std::to_underlying(PinEvent::FallingEdge))) {
    lineRequest.config.flags |= GPIO_V2_LINE_FLAG_EDGE_FALLING;
  }
  if (static_cast<bool>(std::to_underlying(m_info.listenEvents) & std::to_underlying(PinEvent::RisingEdge))) {
    lineRequest.config.flags |= GPIO_V2_LINE_FLAG_EDGE_RISING;
    // seems something is broken in the kernel and notification doesn't properly work without
    // falling edge flag
    lineRequest.config.flags |= GPIO_V2_LINE_FLAG_EDGE_FALLING;
  }

  strncpy(&lineRequest.consumer[0], m_info.label.c_str(), GPIO_MAX_NAME_SIZE - 1);

  int res = ioctl(chipFd.get(), GPIO_V2_GET_LINE_IOCTL, &lineRequest);
  if (res < 0) {
    throw libopenpresso::SystemError{"Failed to set gpio event"};
  }
  return fd_wrapper{lineRequest.fd};
}
