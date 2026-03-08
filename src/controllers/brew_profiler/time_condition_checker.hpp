#ifndef CONTROLLERS_BREW_PROFILER_TIME_CONDITION_CHECKER_HPP
#define CONTROLLERS_BREW_PROFILER_TIME_CONDITION_CHECKER_HPP

#include <chrono>

#include <libopenpresso/types.hpp>

namespace libopenpresso
{

class TimeConditionChecker {
public:
  TimeConditionChecker(time_delta_t duration)
  : m_duration{duration}
  {
  }

  void prepare() noexcept
  {
    m_startTime = std::chrono::steady_clock::now();
  }

  bool isSatisfied() const noexcept
  {
    return std::chrono::steady_clock::now() >= m_startTime + m_duration;
  }

private:
  std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();
  time_delta_t m_duration;
};

} // namespace libopenpresso

#endif // CONTROLLERS_BREW_PROFILER_TIME_CONDITION_CHECKER_HPP