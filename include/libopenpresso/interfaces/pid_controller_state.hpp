/**
 * @file pid_controller_state.hpp
 * @brief Interface for querying PID controller internal state and terms.
 *
 * Defines the interface for read-only access to internal PID calculations
 * for monitoring, debugging, and telemetry purposes.
 */

#ifndef LIBOPENPRESSO_INTERFACES_PID_CONTROLLER_STATE_HPP
#define LIBOPENPRESSO_INTERFACES_PID_CONTROLLER_STATE_HPP

#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @class PidControllerState
 * @brief Read-only interface for PID controller internal state inspection.
 *
 * Provides visibility into the individual PID terms (P, I, D) and their sum
 * for monitoring, tuning validation, and telemetry logging.
 *
 * Useful for:
 * - Tuning PID controller settings
 * - Logging and visualisation with graphing tools
 */

/**
 * @fn virtual pid_calc_t PidControllerState::pTerm() const
 * @brief Get the proportional (P) term contribution.
 *
 * Reflects the immediate reaction proportional to current error.
 *
 * @return P term value calculated as P_gain * error.
 */

/**
 * @fn virtual pid_calc_t PidControllerState::iTerm() const
 * @brief Get the integral (I) term contribution.
 *
 * Reflects accumulated correction for sustained errors over time.
 *
 * @return I term value calculated from integrated error history.
 */

/**
 * @fn virtual pid_calc_t PidControllerState::dTerm() const
 * @brief Get the derivative (D) term contribution.
 *
 * Reflects dampening based on the rate of error change.
 *
 * @return D term value calculated from error rate of change.
 */

/**
 * @fn virtual pid_calc_t PidControllerState::fTerm() const
 * @brief Get the feedforward (F) term contribution.
 *
 * Reflects direct feedforward compensation independent of error.
 *
 * @return F term value calculated from reference signal.
 */

/**
 * @fn virtual pid_calc_t PidControllerState::pidSum() const
 * @brief Get the total PID output sum.
 *
 * The final control signal sent to the actuator, equal to the sum of all terms.
 * This value is typically clamped and converted for the specific actuator.
 *
 * @return Sum of P + I + D + F terms.
 */

class PidControllerState {
public:
  virtual pid_calc_t pTerm() const = 0;

  virtual pid_calc_t iTerm() const = 0;

  virtual pid_calc_t dTerm() const = 0;

  virtual pid_calc_t fTerm() const = 0;

  virtual pid_calc_t pidSum() const = 0;

  virtual ~PidControllerState() = default;
};

} // namespace libopenpresso::interfaces

#endif // LIBOPENPRESSO_INTERFACES_PID_CONTROLLER_STATE_HPP