#include "multi_pin_monitor.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <ranges>
#include <utility>

#include <libopenpresso/exception.hpp>

#include <gpio/pin_data.hpp>
#include <gpio/pin_event_handler.hpp>
#include <sys/epoll.h>
#include <utils/epoll_thread.hpp>

using namespace libopenpresso::gpio;

MultiPinMonitor::MultiPinMonitor(const pins_map_t& pins)
: m_pins{std::ranges::to<event_data_map_t>(pins | std::views::transform(makePinEventPair))}
, m_thread{makeEpollThread()}
{
}

std::shared_ptr<PinMonitor> libopenpresso::gpio::MultiPinMonitor::getPinMonitor(pin_id_t pinId) const
{
  auto pinIt = m_pins.find(pinId);
  if (pinIt == m_pins.end()) {
    throw libopenpresso::Exception{"Requested pin id {} isn't registered for monitoring", pinId};
  }

  return pinIt->second;
}

std::pair<pin_id_t, std::shared_ptr<PinEventHandler>> MultiPinMonitor::makePinEventPair(
  const std::pair<pin_id_t, input_pin_info_t>& pin)
{
  return std::make_pair(pin.first, std::make_shared<PinEventHandler>(pin.second));
}

std::unique_ptr<EpollThread> libopenpresso::gpio::MultiPinMonitor::makeEpollThread() const
{
  auto eventInfoView = m_pins | std::views::transform([](auto&& pin) {
                         return EpollThread::EventInfo{.fd = pin.second->getFd(),
                                                       .events = EPOLLIN,
                                                       .data = {.ptr = pin.second.get()}};
                       });

  return std::make_unique<EpollThread>(eventInfoView, [](uint32_t, epoll_data data) {
    static_cast<PinEventHandler*>(data.ptr)->processEvent();
  });
}
