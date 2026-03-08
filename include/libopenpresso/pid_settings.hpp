/**
 * @file pid_settings.hpp
 * @brief PID controller tuning parameters and filter settings.
 *
 * Defines all tuning coefficients and filter parameters for PID-based controllers.
 */

#ifndef LIBOPENPRESSO_PID_SETTINGS_HPP
#define LIBOPENPRESSO_PID_SETTINGS_HPP

#include <libopenpresso/types.hpp>

namespace libopenpresso
{

/**
 * @struct PidSettings
 * @brief PID controller configuration and tuning parameters.
 *
 * Defines settings for PID controller optimized for high thermal inertia
 * coffee machine boiler heating. PID controller output is calculated as:
 * @f[ \text{sum} = p \cdot err - d_{\text{relaxed}} \cdot \dot{T}_{\text{filtered}} +
 * i_{\text{relaxed}} \cdot \int err
 * \, dt + f \cdot \dot{m} + w \cdot m @f]
 *
 * where:
 * - @f$err\f$ is the temperature error
 * - @f$\dot{T}_{\text{filtered}}\f$ is the low-pass filtered temperature derivative
 * - @f$\int err \, dt\f$ is the integral of error over time
 * - @f$\dot{m}\f$ is the flow rate of cold water that goes into the boiler (derivative of mass)
 * - @f$m\f$ is the weight of cold water that goes into the boiler (mass)
 *
 * Additional settings over p, i and d, present in almost all PID controllers:
 * - D-term low-pass filter: derivative term is a terrible noise-amplifier
 * by its nature, so low-pass filtering is critical for stable operation.
 * - I-term relaxation: i-term should compensate constant heat losses,
 * but it should kick-in only when pid sum isn't maxed out.
 *
 * Coffee machine boiler heating case specific settings:
 * - D-term relaxation: reduces derivative term impact proportionally to the error,
 * allows to set higher damping factor near the setpoint without high affect
 * on the setpoint approach speed.
 * - f: allows to heat boiler right at the monent when we pour cold water into it,
 * preventing temperature drops and further temperature bounces.
 * - w: allows to post-heat cold water after pump was stopped, improves thermal
 * balance recovery after brewing. Works best with f-term.
 * - wDecay: slowly reduces w-term impact when temperature is above the setpoint,
 * is critical to be set if w is non-zero, otherwise boiler will overheat quickly
 *
 * @details
 */
struct PidSettings {
  pid_coeffs_t p; ///< Proportional gain, power per degree (e.g 0.1 means 10% power for 1°C error,
                  ///< 100% power for 10°C error)
  pid_coeffs_t d; ///< Derivative gain, power reduction per degree/second (e.g., 0.1 means -10%
                  ///< power for 1 °C/s temperature rise)
  pid_coeffs_t dTermRelax;  ///< Reduces d-term impact proportinal to the error (e.g 0.01 means
                            ///< \f$d_{\text{relaxed}} = 0.99 \cdot d\f$ for 1°C error,
                            ///< \f$d_{\text{relaxed}} = 0.9 \cdot d\f$ for 10°C error)
  time_delta_t dFilterTime; ///< Low-pass filter time constant for derivative term (about 5-10
                            ///< times of the temperature sensor update period, should be as low
                            ///< as possible to prevent phase lag, but enough to filter d-term
                            ///< amplified noise)
  pid_coeffs_t i; ///< Integral gain, power per degree-second (e.g., 0.001 means +0.1% power for
                  ///< 1°C error each 1 second)
  pid_coeffs_t iTermRelax; ///< Reduces i-term accumulation proportinal to the pid sum (e.g., 0.5
                           ///< means
                           ///< \f$i_{\text{relaxed}} = 0.5 \cdot i\f$ for 100% pid sum, 1.0 means
                           ///< \f$i_{\text{relaxed}} = 0\f$ for 100% pid sum, 2.0 means
                           ///< \f$i_{\text{relaxed}} = 0\f$ for 50% pid sum)
  pid_coeffs_t f; ///< Flow rate feedforward gain, power per grams/second (e.g., 0.05 means +5%
                  ///< power for 1g/s flow rate)
  pid_coeffs_t w; ///< Weight feedforward gain, power per gram (e.g., 0.001 means +0.1% power for
                  ///< each 1g weight added)
  pid_coeffs_t wDecay; ///< Decay rate for w-term, power reduction per degree-second (e.g., 0.01
                       ///< means -1% power for 1°C overheat each 1 second)
};

} // namespace libopenpresso

#endif // LIBOPENPRESSO_PID_SETTINGS_HPP