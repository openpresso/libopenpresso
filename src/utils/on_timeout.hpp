#ifndef UTILS_ON_TIMEOUT_HPP
#define UTILS_ON_TIMEOUT_HPP

#include <chrono>
#include <functional>
#include <future>

class TaskOnTimeout {
public:
  template <class Rep, class Period, typename F, typename... Args>
  TaskOnTimeout(const std::chrono::duration<Rep, Period>& timeout_duration, F&& func, Args&&... args)
  : m_result(launch(timeout_duration, std::forward<F>(func), std::forward<Args>(args)...))
  {
  }

  TaskOnTimeout(const TaskOnTimeout&) = delete;
  TaskOnTimeout(TaskOnTimeout&&) = delete;
  auto operator=(const TaskOnTimeout&) = delete;
  auto operator=(TaskOnTimeout&&) = delete;

  ~TaskOnTimeout()
  {
    m_cancel.set_value();
  }

private:
  template <class Rep, class Period, typename F, typename... Args>
  static void task(std::future<void> cancel,
                   const std::chrono::duration<Rep, Period>& timeout_duration,
                   F&& func,
                   Args&&... args)
  {
    if (cancel.wait_for(timeout_duration) == std::future_status::timeout) {
      std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
    }
  }

  template <class Rep, class Period, typename F, typename... Args>
  std::future<void> launch(const std::chrono::duration<Rep, Period>& timeout_duration, F&& func, Args&&... args)
  {
    return std::async(std::launch::async,
                      task<Rep, Period, F, Args...>,
                      m_cancel.get_future(),
                      timeout_duration,
                      std::forward<F>(func),
                      std::forward<Args>(args)...);
  }

private:
  std::promise<void> m_cancel;
  std::future<void> m_result;
};

#endif // UTILS_ON_TIMEOUT_HPP