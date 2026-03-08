/**
 * @file brew_steps_data.hpp
 * @brief Brew profile step definitions and transition conditions.
 *
 * Defines the structure and components of automated espresso extraction profiles,
 * including step targets (pressure/flow) and transition conditions.
 */

#ifndef LIBOPENPRESSO_BREW_STEPS_DATA_HPP
#define LIBOPENPRESSO_BREW_STEPS_DATA_HPP

#include <variant>

#include <libopenpresso/types.hpp>

/**
 * @brief Brew profile conditions for advancing to the next step.
 */
namespace libopenpresso::brew_step_advance_conditions
{

/**
 * @struct OnWeight
 * @brief Transition condition: proceed when target weight is reached.
 *
 * Used to advance to next step or stop brewing when output weight reaches specified value.
 */
struct OnWeight {
  milligrams_t weight; ///< Target output weight in milligrams
};

/**
 * @struct OnStepTime
 * @brief Transition condition: proceed after step duration expires.
 *
 * Used to advance to next step or stop brewing after a fixed time within the current step.
 */
struct OnStepTime {
  time_delta_t time; ///< Duration for current step
};

/**
 * @struct OnTotalTime
 * @brief Transition condition: proceed after total brew time expires.
 *
 * Used to advance to next step or stop brewing when total brew time since start is reached.
 */
struct OnTotalTime {
  time_delta_t time; ///< Total elapsed time from brew start
};

/**
 * @struct Never
 * @brief Transition condition: step continues indefinitely.
 *
 * Shouldn't be used for non-last step, otherwise all the following steps become unreachable.
 */
struct Never {};

} // namespace libopenpresso::brew_step_advance_conditions

/**
 * @brief Brew profile step targets.
 */
namespace libopenpresso::brew_step_targets
{

/**
 * @struct ConstantPressure
 * @brief Step target: maintain constant pressure.
 *
 * Pressure-based extraction step that modulates pump power to maintain target pressure.
 */
struct ConstantPressure {
  millibars_t pressure; ///< Target pressure in millibars
};

/**
 * @struct ConstantFlow
 * @brief Step target: maintain constant flow rate.
 *
 * Flow-based extraction step that modulates pump power to maintain target flow rate.
 */
struct ConstantFlow {
  milligrams_p_second_t rate; ///< Target flow rate in milligrams per second
};

} // namespace libopenpresso::brew_step_targets

namespace libopenpresso
{

/**
 * @typedef next_step_condition_t
 * @brief Variant of all possible brew profile step transition conditions.
 *
 * Determines when to advance to the next step in a brew profile.
 * @see brew_step_advance_conditions::OnWeight, brew_step_advance_conditions::OnStepTime,
 * brew_step_advance_conditions::OnTotalTime, brew_step_advance_conditions::Never
 */
using next_step_condition_t = std::variant<brew_step_advance_conditions::OnWeight,
                                           brew_step_advance_conditions::OnStepTime,
                                           brew_step_advance_conditions::OnTotalTime,
                                           brew_step_advance_conditions::Never>;

/**
 * @typedef step_target_t
 * @brief Variant of all possible brew profile step control targets.
 *
 * Specifies what physical parameter is controlled during the step.
 * @see brew_step_targets::ConstantPressure, brew_step_targets::ConstantFlow
 */
using step_target_t = std::variant<brew_step_targets::ConstantPressure, brew_step_targets::ConstantFlow>;

} // namespace libopenpresso

#endif // LIBOPENPRESSO_BREW_STEPS_DATA_HPP