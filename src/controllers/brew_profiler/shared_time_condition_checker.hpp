#ifndef CONTROLLERS_BREW_PROFILER_SHARED_TIME_CONDITION_CHECKER_HPP
#define CONTROLLERS_BREW_PROFILER_SHARED_TIME_CONDITION_CHECKER_HPP

#include <chrono>
#include <memory>

#include <libopenpresso/types.hpp>

namespace libopenpresso
{

class SharedTimeConditionChecker {
public:
  SharedTimeConditionChecker(const std::shared_ptr<std::chrono::steady_clock::time_point>& startTime,
                             time_delta_t duration)
  : m_startTime{startTime}
  , m_duration{duration}
  {
  }

  void prepare()
  {
  }

  bool isSatisfied() const
  {
    return std::chrono::steady_clock::now() >= *m_startTime + m_duration;
  }

private:
  std::shared_ptr<std::chrono::steady_clock::time_point> m_startTime;
  time_delta_t m_duration;
};

} // namespace libopenpresso

#endif // CONTROLLERS_BREW_PROFILER_SHARED_TIME_CONDITION_CHECKER_HPP