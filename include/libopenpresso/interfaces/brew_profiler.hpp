/**
 * @file brew_profiler.hpp
 * @brief Interface for automated brew profile execution.
 *
 * Defines the interface for brew profiler that executes multi-step
 * espresso extraction sequence with configurable pressure/flow targets and
 * transition conditions.
 */

#ifndef LIBOPENPRESSO_INTERFACES_BREW_PROFILER_HPP
#define LIBOPENPRESSO_INTERFACES_BREW_PROFILER_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include <libopenpresso/brew_steps_data.hpp>
#include <libopenpresso/interfaces/controller_base.hpp>
#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @typedef BrewProfiler::step_index_t
 * @brief Index type for identifying brew profile steps.
 */

/**
 * @typedef BrewProfiler::stopped_flag_t
 * @brief Marker type to indicate brewing has stopped.
 */

/**
 * @typedef BrewProfiler::callback_t
 * @brief Callback function type for step changes.
 *
 * Called with either a step index (step_index_t) for normal progression or
 * stopped_flag_t to indicate brewing has stopped.
 */

/**
 * @class BrewProfiler
 * @brief Interface for automated brew profile execution with step-based control flow.
 *
 * Performs execution of user-defined brewing profiles with multiple steps,
 * each with a specific control target (constant pressure or constant flow) and
 * advancement condition (weight reached, time elapsed, etc.).
 *
 * Inherits activation control from ControllerBase. While active, notifies clients
 * of step changes via callbacks.
 *
 * @note Supported implementation configs: BrewProfilerConfig
 *
 * @note When activated, BrewProfiler tares the associated weight sensor
 *
 * @see ControllerBase, brew_steps_data.hpp
 */

/**
 * @fn virtual void BrewProfiler::setAutoStopCondition(brew_step_advance_conditions::OnWeight)
 * @brief Set the auto-stop condition based on the espresso dose weight.
 *
 * With this condition set, brewing stops automatically when the output weight reaches
 * the specified value. Brew process still can be stopped
 * earlier if the last step in the sequence reaches its transition condition or
 * deactivate() was called manually.
 *
 * @param[in] condition brew_step_advance_conditions::OnWeight condition with target weight in
 * milligrams.
 *
 * @see brew_step_advance_conditions::OnWeight
 */

/**
 * @fn virtual void BrewProfiler::setAutoStopCondition(brew_step_advance_conditions::OnTotalTime)
 * @brief Set the auto-stop condition based on the elapsed brew time.
 *
 * With this condition set, brewing stops automatically when total elapsed
 * time since brew start reaches the specified duration. Brew process still can be stopped
 * earlier if the last step in the sequence reaches its transition condition or
 * deactivate() was called manually.
 *
 * @param[in] condition brew_step_advance_conditions::OnTotalTime condition with max duration.
 *
 * @see brew_step_advance_conditions::OnTotalTime
 */

/**
 * @fn virtual void BrewProfiler::setAutoStopCondition(brew_step_advance_conditions::Never)
 * @brief Disable auto-stop condition (manual stop only).
 *
 * Brewing continues indefinitely until manually stopped via deactivate()
 * or the last step in the sequence reaches its transition condition.
 *
 * @param[in] condition brew_step_advance_conditions::Never unreachable condition (disables
 * auto-stop).
 *
 * @see brew_step_advance_conditions::Never
 */

/**
 * @fn virtual void BrewProfiler::setSteps(const std::vector<std::pair<step_target_t,
 * next_step_condition_t>>&)
 * @brief Set the sequence of brew profile steps.
 *
 * Each step defines a target (constant pressure or flow) and a condition
 * for advancing to the next step. Steps are executed in sequence.
 *
 * @param[in] steps Vector of target + transition condition pairs
 *
 * @see brew_steps_data.hpp for step definitions
 */

/**
 * @fn virtual callback_descriptor_t BrewProfiler::registerStepChangeCallback(const callback_t&)
 * @brief Register a callback for step change notifications.
 *
 * The callback is invoked whenever the profiler advances to a new step
 * (receiving the new step index) or completes brewing (receiving stopped_flag_t{}).
 *
 * @param[in] callback Function to invoke on step changes.
 * @return callback_descriptor_t Descriptor for later unsubscribing from callbacks.
 *
 * @see unregisterStepChangeCallback()
 */

/**
 * @fn virtual void BrewProfiler::unregisterStepChangeCallback(callback_descriptor_t)
 * @brief Unregister a previously registered step change callback.
 *
 * @param[in] descriptor Descriptor returned from registerStepChangeCallback().
 *
 * @see registerStepChangeCallback()
 */

class BrewProfiler : public ControllerBase {
public:
  using step_index_t = size_t;

  using stopped_flag_t = std::monostate;

  using callback_t = std::function<void(std::variant<step_index_t, stopped_flag_t>)>;

  virtual void setAutoStopCondition(brew_step_advance_conditions::OnWeight condition) = 0;

  virtual void setAutoStopCondition(brew_step_advance_conditions::OnTotalTime condition) = 0;

  virtual void setAutoStopCondition(brew_step_advance_conditions::Never condition) = 0;

  virtual void setSteps(const std::vector<std::pair<step_target_t, next_step_condition_t>>& steps) = 0;

  virtual callback_descriptor_t registerStepChangeCallback(const callback_t& callback) = 0;

  virtual void unregisterStepChangeCallback(callback_descriptor_t descriptor) = 0;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using BrewProfilerPtr = std::shared_ptr<interfaces::BrewProfiler>; ///< Alias to BrewProfiler shared
                                                                   ///< pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_BREW_PROFILER_HPP