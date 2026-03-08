#include "debounced_pin_state.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <gpio/pin_monitor.hpp>

using namespace libopenpresso;

SwDebouncedButtonState::SwDebouncedButtonState(const std::shared_ptr<gpio::PinMonitor>& monitor,
                                               time_delta_t debounce,
                                               bool inverted)
: m_isPressed{monitor->readState() == (inverted ? gpio::PinState::Low : gpio::PinState::High)}
, m_inverted(inverted)
, m_timeout{debounce}
, m_monitorCallback{
    monitor,
    gpio::PinEvent::Both,
    debounce.count() != 0
      ? gpio::PinMonitor::PinEventCallback_t{[this](gpio::PinEvent evt) { debounceCallback(evt); }}
      : gpio::PinMonitor::PinEventCallback_t{[this](gpio::PinEvent evt) { noDebounceCallback(evt); }}}
{
}

void libopenpresso::SwDebouncedButtonState::noDebounceCallback(gpio::PinEvent event)
{
  const bool state =
    m_inverted ? (event == gpio::PinEvent::FallingEdge) : (event == gpio::PinEvent::RisingEdge);
  const auto oldState = m_isPressed.exchange(state, std::memory_order_relaxed);

  if (oldState == state) {
    return;
  }

  std::scoped_lock lock(m_callbacksLock);
  for (auto&& cb : m_callbacks) {
    std::invoke(cb.second, state);
  }
}

void libopenpresso::SwDebouncedButtonState::debounceCallback(gpio::PinEvent event)
{
  const bool state =
    m_inverted ? (event == gpio::PinEvent::FallingEdge) : (event == gpio::PinEvent::RisingEdge);

  m_delayedCallback.emplace(m_timeout, [this, state]() {
    const auto oldState = m_isPressed.exchange(state, std::memory_order_relaxed);
    if (oldState == state) {
      return;
    }

    std::scoped_lock lock(m_callbacksLock);
    for (auto&& cb : m_callbacks) {
      std::invoke(cb.second, state);
    }
  });
}

callback_descriptor_t SwDebouncedButtonState::registerCallback(const callback_t& callback)
{
  if (!callback) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks.insert({m_nextCallbackDescriptor, callback});
  return m_nextCallbackDescriptor++;
}

void libopenpresso::SwDebouncedButtonState::unregisterCallback(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_callbacksLock);

  if (auto it = m_callbacks.find(descr); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

bool SwDebouncedButtonState::getState() const
{
  return m_isPressed.load(std::memory_order_relaxed);
}

std::optional<time_delta_t> libopenpresso::SwDebouncedButtonState::fixedUpdateRate() const noexcept
{
  return {};
}
