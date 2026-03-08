#include "brew_profiler_impl.hpp"

#include "brew_profiler/const_param_executor.hpp"
#include "brew_profiler/ureachable_condition_checker.hpp"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <future>
#include <mutex>
#include <ranges>
#include <utility>
#include <variant>
#include <vector>

#include <libopenpresso/brew_steps_data.hpp>
#include <libopenpresso/exception.hpp>
#include <libopenpresso/interfaces/controller_base.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/logical_output.hpp>
#include <libopenpresso/interfaces/pressure_controller.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <utils/logger.hpp>

using namespace libopenpresso;

BrewProfilerImpl::BrewProfilerImpl(time_delta_t updatePeriod,
                                   PressureControllerPtr pressureController,
                                   FlowRateControllerPtr flowController,
                                   WeightSensorPtr weightSensor,
                                   LogicalOutputPtr valve)
: m_updatePeriod{updatePeriod}
, m_pressureController{std::move(pressureController)}
, m_flowController{std::move(flowController)}
, m_weightSensor{std::move(weightSensor)}
, m_valve{std::move(valve)}
{
}

BrewProfilerImpl::~BrewProfilerImpl()
{
  if (!isActive()) {
    return;
  }

  m_exit.set_value();
  m_transitionThread.join();
}

void BrewProfilerImpl::setAutoStopCondition(brew_step_advance_conditions::OnWeight condition)
{
  m_stopCondition = weight_condition_checker_t{m_weightSensor, condition.weight};
}

void BrewProfilerImpl::setAutoStopCondition(brew_step_advance_conditions::OnTotalTime condition)
{
  m_stopCondition = SharedTimeConditionChecker{m_startTime, condition.time};
}

void BrewProfilerImpl::setAutoStopCondition([[maybe_unused]] brew_step_advance_conditions::Never never)
{
  m_stopCondition = UnreachableConditionChecker{};
}

void libopenpresso::BrewProfilerImpl::setSteps(
  const std::vector<std::pair<step_target_t, next_step_condition_t>>& stepsConfig)
{
  auto executor = [this](auto&& step) { return makeStepExecutor(step); };
  auto condition = [this](auto&& step) { return makeConditionChecker(step); };
  auto transformer = [executor, condition](auto&& step) {
    return std::make_pair(std::visit(executor, step.first), std::visit(condition, step.second));
  };

  m_steps = std::ranges::to<steps_vector_t>(stepsConfig | std::views::transform(transformer));
}

void BrewProfilerImpl::activate()
{
  if (isActive()) {
    throw libopenpresso::Exception{"Brew profiler is already active"};
  }

  if (m_steps.empty()) {
    throw libopenpresso::Exception{"No brew steps added"};
  }

  m_exit = {};
  m_transitionThread = std::thread{&BrewProfilerImpl::transitionWorker, this, m_exit.get_future()};
  m_isActive.store(true, std::memory_order_relaxed);
}

void BrewProfilerImpl::deactivate()
{
  if (!isActive()) {
    return;
  }

  m_exit.set_value();
  m_transitionThread.join();
  m_isActive.store(false, std::memory_order_relaxed);
}

bool BrewProfilerImpl::isActive() const noexcept
{
  return m_isActive.load(std::memory_order_relaxed);
}

callback_descriptor_t BrewProfilerImpl::registerStepChangeCallback(const callback_t& callback)
{
  if (!callback) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks.emplace(m_nextCallbackDescriptor, callback);
  return m_nextCallbackDescriptor++;
}

void BrewProfilerImpl::unregisterStepChangeCallback(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_callbacksLock);
  if (auto it = m_callbacks.find(descr); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

void BrewProfilerImpl::notifyCallbacks(auto info)
{
  std::scoped_lock lock(m_callbacksLock);
  for (auto&& cb : m_callbacks) {
    std::invoke(cb.second, info);
  }
}

ConstParamExecutor BrewProfilerImpl::makeStepExecutor(brew_step_targets::ConstantPressure pressure)
{
  return ConstParamExecutor{
    m_pressureController, pressure.pressure, &interfaces::PressureController::setTargetPressure};
}

ConstParamExecutor BrewProfilerImpl::makeStepExecutor(brew_step_targets::ConstantFlow flow)
{
  return ConstParamExecutor{m_flowController, flow.rate, &interfaces::FlowRateController::setTargetRate};
}

BrewProfilerImpl::transition_condition_checker_t BrewProfilerImpl::makeConditionChecker(
  brew_step_advance_conditions::OnWeight condition)
{
  return weight_condition_checker_t{m_weightSensor, condition.weight};
}

BrewProfilerImpl::transition_condition_checker_t BrewProfilerImpl::makeConditionChecker(
  brew_step_advance_conditions::OnTotalTime condition)
{
  return SharedTimeConditionChecker{m_startTime, condition.time};
}

// NOLINTBEGIN(readability-convert-member-functions-to-static)
BrewProfilerImpl::transition_condition_checker_t BrewProfilerImpl::makeConditionChecker(
  brew_step_advance_conditions::OnStepTime condition)
{
  return TimeConditionChecker{condition.time};
}

BrewProfilerImpl::transition_condition_checker_t BrewProfilerImpl::makeConditionChecker(
  [[maybe_unused]] brew_step_advance_conditions::Never never)
{
  return UnreachableConditionChecker{};
}
// NOLINTEND(readability-convert-member-functions-to-static)

bool BrewProfilerImpl::waitStop(const std::future<void>& exit, transition_condition_checker_t& cond)
{
  while (exit.wait_for(m_updatePeriod) == std::future_status::timeout &&
         !std::visit(conditionPred, m_stopCondition)) {
    if (std::visit(conditionPred, cond)) {
      return false;
    }
  }
  return true;
}

void BrewProfilerImpl::transitionWorker(const std::future<void>& exit)
{
  try {
    *m_startTime = std::chrono::steady_clock::now();
    m_weightSensor->tare();

    m_valve->activate();
    m_valve->setState(true);

    executeSteps(exit);

    m_valve->deactivate();
    notifyCallbacks(stopped_flag_t{});
  }
  catch (const libopenpresso::Exception& e) {
    Logger::critical("Brew profiler thread raised an exception, reason: {}, thrown from "
                     "file: {}, function: {}, line: {}",
                     e.what(),
                     e.throwLocation().file_name(),
                     e.throwLocation().function_name(),
                     e.throwLocation().line());
    std::abort();
  }
  catch (const std::exception& e) {
    Logger::critical("Brew profiler thread raised an exception, reason: {}", e.what());
    std::abort();
  }
  catch (...) {
    Logger::critical("Brew profiler thread raised an unknow exception");
    std::abort();
  }
}

void BrewProfilerImpl::executeSteps(const std::future<void>& exit)
{
  size_t stepIndex = 0;
  interfaces::ControllerBase* prevStepController = nullptr;

  for (auto&& step : m_steps) {
    std::visit([](auto&& cond) { cond.prepare(); }, step.second);
    notifyCallbacks(stepIndex++);

    step.first.setTargetValue();
    if (step.first.controllerBase().get() != prevStepController) {
      if (prevStepController != nullptr) {
        prevStepController->deactivate();
      }
      step.first.controllerBase()->activate();
      prevStepController = step.first.controllerBase().get();
    }

    if (waitStop(exit, step.second)) {
      break;
    }
  }

  if (prevStepController != nullptr) {
    prevStepController->deactivate();
  }
}