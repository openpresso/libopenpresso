/**
 * @file flow_rate_controller.hpp
 * @brief Interface for flow rate regulation controllers.
 *
 * Defines the interface for controllers that regulate water flow rate
 * through the espresso machine (pump, solenoid, etc.).
 */

#ifndef LIBOPENPRESSO_INTERFACES_FLOW_RATE_CONTROLLER_HPP
#define LIBOPENPRESSO_INTERFACES_FLOW_RATE_CONTROLLER_HPP

#include <memory>

#include <libopenpresso/interfaces/controller_base.hpp>
#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @class FlowRateController
 * @brief Interface for flow rate regulation with setpoint control.
 *
 * Controls water flow rate toward a target by modulating pump speed,
 * solenoid opening, or proportional valve position. Inherits activation
 * control from ControllerBase.
 *
 * @note Supported implementation configs: VibroPumpFlowController, IntegralFlowRateControllerConfig
 *
 * @see ControllerBase
 */

/**
 * @fn virtual milligrams_p_second_t FlowRateController::getTargetRate() const
 * @brief Get the current flow rate target setpoint.
 *
 * @return Current target flow rate in milligrams per second.
 */

/**
 * @fn virtual void FlowRateController::setTargetRate(milligrams_p_second_t)
 * @brief Set the flow rate target setpoint.
 *
 * When the controller is active, it will regulate flow rate toward this value.
 *
 * @param[in] rate Target flow rate in milligrams per second.
 */

class FlowRateController : public ControllerBase {
public:
  virtual milligrams_p_second_t getTargetRate() const = 0;
  virtual void setTargetRate(milligrams_p_second_t rate) = 0;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using FlowRateControllerPtr = std::shared_ptr<interfaces::FlowRateController>; ///< Alias to
                                                                               ///< FlowRateController
                                                                               ///< shared pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_FLOW_RATE_CONTROLLER_HPP