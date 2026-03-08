#ifndef UTILS_EPOLL_THREAD_HPP
#define UTILS_EPOLL_THREAD_HPP

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <functional>
#include <pthread.h>
#include <ranges>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

#include <libopenpresso/exception.hpp>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <utils/fd_wrapper.hpp>
#include <utils/input_range_of.hpp>
#include <utils/logger.hpp>

class EpollThread {
public:
  struct EventInfo {
    int fd;
    uint32_t events;
    epoll_data data;
  };

public:
  template <input_range_of<EventInfo> Range, typename Cb>
  EpollThread(const Range& events, Cb&& callback)
  : m_stopEvent(eventfd(0, 0))
  {
    if (std::ranges::empty(events)) {
      throw libopenpresso::Exception{"Monitor has no added events"};
    }

    std::vector<epoll_event> eventsStorage{std::ranges::size(events) + 1};

    auto epollFd = fd_wrapper{epoll_create1(0)};
    epollAddDescriptor(
      epollFd.get(),
      {.fd = m_stopEvent.get(), .events = EPOLLIN, .data = epoll_data{.fd = m_stopEvent.get()}});

    std::ranges::for_each(events, [fd = epollFd.get()](auto&& evt) { epollAddDescriptor(fd, evt); });

    m_monitorThread = std::thread(
      makeWorker(std::move(epollFd), std::ranges::size(events) + 1, std::forward<Cb>(callback)));
  }

  EpollThread(const EpollThread&) = delete;
  EpollThread(EpollThread&&) = delete;
  auto operator=(const EpollThread&) -> EpollThread& = delete;
  auto operator=(EpollThread&&) -> EpollThread& = delete;
  ~EpollThread()
  {
    if (!m_monitorThread.joinable()) {
      return;
    }

    eventfd_t val = 1;
    eventfd_write(m_stopEvent, val);
    m_monitorThread.join();
  }

  void setSchedParams(int priority, int policy)
  {
    sched_param sch_params{.sched_priority = priority};
    int result = pthread_setschedparam(m_monitorThread.native_handle(), policy, &sch_params);
    if (result != 0) {
      throw libopenpresso::SystemError{"Failed to set thread priority"};
    }
  }

private:
  static void epollAddDescriptor(int epfd, EventInfo event)
  {
    epoll_event ev = {.events = event.events, .data = event.data};
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, event.fd, &ev) != 0) {
      throw libopenpresso::SystemError{"Failed to add descriptor to epoll"};
    }
  }

  template <typename Cb>
  auto makeWorker(fd_wrapper&& epoll, size_t eventsCount, Cb&& callback)
  {
    return [this,
            epoll = std::move(epoll),
            events = std::vector<epoll_event>{eventsCount},
            callback = std::forward<Cb>(callback)] mutable {
      bool exit = false;
      while (!exit) {
        int nfds = epoll_wait(epoll.get(), events.data(), events.size(), -1);
        if (nfds < 0) {
          if (errno == EINTR) {
            continue;
          }
          std::error_code err = {errno, std::system_category()};
          libopenpresso::Logger::critical("Epoll wait error, code: {}, message", err.value(), err.message());
          std::abort();
        }

        for (int i = 0; i < nfds; ++i) {
          if (events[i].data.fd == m_stopEvent.get()) {
            exit = true;
            break;
          }

          try {
            auto eventsMask = events[i].events;
            auto eventData = events[i].data;
            std::invoke(callback, eventsMask, eventData);
          }
          catch (const libopenpresso::Exception& e) {
            libopenpresso::Logger::err("Epoll event handler exception caught, what: {}, thrown "
                                       "from file: {}, function: {}, line: {}",
                                       e.what(),
                                       e.throwLocation().file_name(),
                                       e.throwLocation().function_name(),
                                       e.throwLocation().line());
          }
          catch (std::exception& e) {
            libopenpresso::Logger::err("Epoll event handler exception caught, what: {}", e.what());
          }
        }
      }
    };
  }

private:
  const fd_wrapper m_stopEvent;
  std::thread m_monitorThread;
};

#endif // UTILS_EPOLL_THREAD_HPP