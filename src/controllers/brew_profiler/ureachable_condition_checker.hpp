#ifndef CONTROLLERS_BREW_PROFILER_UREACHABLE_CONDITION_CHECKER_HPP
#define CONTROLLERS_BREW_PROFILER_UREACHABLE_CONDITION_CHECKER_HPP

namespace libopenpresso
{

class UnreachableConditionChecker {
public:
  void prepare() const noexcept
  {
  }
  bool isSatisfied() const noexcept
  {
    (void)this;
    return false;
  }
};

} // namespace libopenpresso

#endif // CONTROLLERS_BREW_PROFILER_UREACHABLE_CONDITION_CHECKER_HPP