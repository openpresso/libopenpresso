#ifndef CONTROLLERS_BREW_PROFILER_BREW_PROFILER_IMPL_HPP
#define CONTROLLERS_BREW_PROFILER_BREW_PROFILER_IMPL_HPP

#include "const_param_executor.hpp"
#include "sensor_condition_checker.hpp"
#include "shared_time_condition_checker.hpp"
#include "time_condition_checker.hpp"
#include "ureachable_condition_checker.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <libopenpresso/brew_steps_data.hpp>
#include <libopenpresso/interfaces/brew_profiler.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/logical_output.hpp>
#include <libopenpresso/interfaces/pressure_controller.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <utils/spinlock.hpp>

namespace libopenpresso
{

namespace interfaces
{
class WeightSensor;
class PressureController;
class FlowRateController;
class LogicalOutput;
} // namespace interfaces

class BrewProfilerImpl : public interfaces::BrewProfiler {
  using weight_condition_checker_t =
    SensorConditionChecker<interfaces::WeightSensor, milligrams_t, &interfaces::WeightSensor::getWeight, std::less{}>;
  using stop_condition_checker_t =
    std::variant<weight_condition_checker_t, SharedTimeConditionChecker, UnreachableConditionChecker>;
  using transition_condition_checker_t =
    std::variant<weight_condition_checker_t, SharedTimeConditionChecker, TimeConditionChecker, UnreachableConditionChecker>;
  using steps_vector_t = std::vector<std::pair<ConstParamExecutor, transition_condition_checker_t>>;

  static constexpr auto conditionPred = [](auto&& cond) { return cond.isSatisfied(); };

public:
  BrewProfilerImpl(time_delta_t updatePeriod,
                   PressureControllerPtr pressureController,
                   FlowRateControllerPtr flowController,
                   WeightSensorPtr weightSensor,
                   LogicalOutputPtr valve);
  BrewProfilerImpl(const BrewProfilerImpl&) = delete;
  BrewProfilerImpl(BrewProfilerImpl&&) = delete;
  auto operator=(const BrewProfilerImpl&) = delete;
  auto operator=(BrewProfilerImpl&&) = delete;
  ~BrewProfilerImpl();
  void setAutoStopCondition(brew_step_advance_conditions::OnWeight condition) override;
  void setAutoStopCondition(brew_step_advance_conditions::OnTotalTime condition) override;
  void setAutoStopCondition(brew_step_advance_conditions::Never never) override;

  void setSteps(const std::vector<std::pair<step_target_t, next_step_condition_t>>& steps) override;

  void activate() override;
  void deactivate() override;
  bool isActive() const noexcept override;

  callback_descriptor_t registerStepChangeCallback(const callback_t& callback) override;
  void unregisterStepChangeCallback(callback_descriptor_t descr) override;

private:
  void transitionWorker(const std::future<void>& exit);
  void executeSteps(const std::future<void>& exit);
  void notifyCallbacks(auto info);

  ConstParamExecutor makeStepExecutor(brew_step_targets::ConstantPressure pressure);
  ConstParamExecutor makeStepExecutor(brew_step_targets::ConstantFlow flow);
  transition_condition_checker_t makeConditionChecker(brew_step_advance_conditions::OnWeight condition);
  transition_condition_checker_t makeConditionChecker(brew_step_advance_conditions::OnTotalTime condition);
  transition_condition_checker_t makeConditionChecker(brew_step_advance_conditions::OnStepTime condition);
  transition_condition_checker_t makeConditionChecker(brew_step_advance_conditions::Never never);

  bool waitStop(const std::future<void>& exit, transition_condition_checker_t& cond);

private:
  steps_vector_t m_steps;
  stop_condition_checker_t m_stopCondition = UnreachableConditionChecker{};

  std::shared_ptr<std::chrono::steady_clock::time_point> m_startTime =
    std::make_shared<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());

  std::promise<void> m_exit;
  std::thread m_transitionThread;
  const time_delta_t m_updatePeriod;
  std::atomic<bool> m_isActive = false;

  const PressureControllerPtr m_pressureController;
  const FlowRateControllerPtr m_flowController;
  const WeightSensorPtr m_weightSensor;
  const LogicalOutputPtr m_valve;

  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, callback_t> m_callbacks;
};

} // namespace libopenpresso

#endif // CONTROLLERS_BREW_PROFILER_BREW_PROFILER_IMPL_HPP