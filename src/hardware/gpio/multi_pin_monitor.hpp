#ifndef HARDWARE_GPIO_MULTI_PIN_MONITOR_HPP
#define HARDWARE_GPIO_MULTI_PIN_MONITOR_HPP

#include <memory>
#include <unordered_map>
#include <utility>

#include <gpio/pin_data.hpp>
#include <utils/epoll_thread.hpp>

struct epoll_event;

namespace libopenpresso::gpio
{

class PinMonitor;
class PinEventHandler;

class MultiPinMonitor {
  using event_data_map_t = std::unordered_map<pin_id_t, std::shared_ptr<PinEventHandler>>;

public:
  using pins_map_t = std::unordered_map<pin_id_t, input_pin_info_t>;

public:
  MultiPinMonitor(const pins_map_t& pins);
  MultiPinMonitor(const MultiPinMonitor&) = delete;
  MultiPinMonitor(MultiPinMonitor&&) = default;
  auto operator=(const MultiPinMonitor&) -> MultiPinMonitor& = delete;
  auto operator=(MultiPinMonitor&&) -> MultiPinMonitor& = default;
  ~MultiPinMonitor() = default;

  std::shared_ptr<PinMonitor> getPinMonitor(pin_id_t pinId) const;

private:
  static std::pair<pin_id_t, std::shared_ptr<PinEventHandler>> makePinEventPair(
    const std::pair<pin_id_t, input_pin_info_t>& pin);
  std::unique_ptr<EpollThread> makeEpollThread() const;

private:
  event_data_map_t m_pins;
  std::unique_ptr<EpollThread> m_thread;
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_MULTI_PIN_MONITOR_HPP