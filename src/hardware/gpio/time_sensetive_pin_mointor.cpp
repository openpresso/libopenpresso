#include "time_sensetive_pin_mointor.hpp"

#include <cstdint>
#include <ranges>
#include <sched.h>

#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <sys/epoll.h>

using namespace libopenpresso::gpio;

TimeSensetivePinMonitor::TimeSensetivePinMonitor(const input_pin_info_t& pin)
: m_eventHandler{pin}
, m_thread{std::views::single(EpollThread::EventInfo{
             .fd = m_eventHandler.getFd(), .events = EPOLLIN, .data = {.ptr = &m_eventHandler}}),
           [](uint32_t, epoll_data data) { static_cast<PinEventHandler*>(data.ptr)->processEvent(); }}
{
  m_thread.setSchedParams(THREAD_PRIORITY, SCHED_FIFO);
}

libopenpresso::callback_descriptor_t TimeSensetivePinMonitor::registerCallback(
  PinEvent notifyOn, const PinEventCallback_t& callback)
{
  return m_eventHandler.registerCallback(notifyOn, callback);
}

void TimeSensetivePinMonitor::unregisterCallback(callback_descriptor_t callbackDescriptor)
{
  m_eventHandler.unregisterCallback(callbackDescriptor);
}

PinState TimeSensetivePinMonitor::readState() const
{
  return m_eventHandler.readState();
}
