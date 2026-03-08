#ifndef CONTROLLERS_BREW_PROFILER_CONST_PARAM_EXECUTOR_HPP
#define CONTROLLERS_BREW_PROFILER_CONST_PARAM_EXECUTOR_HPP

#include <functional>
#include <memory>

#include <libopenpresso/interfaces/controller_base.hpp>

namespace libopenpresso
{

class ConstParamExecutor {
public:
  template <typename Controller, typename Value>
  ConstParamExecutor(const std::shared_ptr<Controller>& controller,
                     Value val,
                     void (Controller::*setter)(Value))
  : m_targetValueSetter{[val, controller, setter] { std::invoke(setter, controller.get(), val); }}
  , m_controller{controller}
  {
  }

  void setTargetValue()
  {
    std::invoke(m_targetValueSetter);
  }

  const ControllerBasePtr& controllerBase() const noexcept
  {
    return m_controller;
  }

private:
  std::function<void()> m_targetValueSetter;
  ControllerBasePtr m_controller;
};

} // namespace libopenpresso

#endif // CONTROLLERS_BREW_PROFILER_CONST_PARAM_EXECUTOR_HPP