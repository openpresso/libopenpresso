/**
 * @file pressure_controller.hpp
 * @brief Interface for pressure regulation controllers.
 *
 * Defines the interface for controllers that regulate system pressure
 * (group head, pump outlet pipe, or boiler).
 */

#ifndef LIBOPENPRESSO_INTERFACES_PRESSURE_CONTROLLER_HPP
#define LIBOPENPRESSO_INTERFACES_PRESSURE_CONTROLLER_HPP

#include <memory>

#include <libopenpresso/interfaces/controller_base.hpp>
#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @class PressureController
 * @brief Interface for pressure regulation with setpoint control.
 *
 * Controls system pressure toward a target setpoint by modulating
 * pump output or solenoid valves. Inherits activation control from ControllerBase.
 *
 * @note Supported implementation configs: PulsePressureControllerConfig
 *
 * @see ControllerBase
 */

/**
 * @fn virtual millibars_t PressureController::getTargetPressure() const
 * @brief Get the current pressure target setpoint.
 *
 * @return Current target pressure in millibars.
 */

/**
 * @fn virtual void PressureController::setTargetPressure(millibars_t)
 * @brief Set the pressure target setpoint.
 *
 * When the controller is active, it will regulate pressure toward this value.
 *
 * @param[in] millibars Target pressure in millibars.
 */

class PressureController : public ControllerBase {
public:
  virtual millibars_t getTargetPressure() const = 0;
  virtual void setTargetPressure(millibars_t millibars) = 0;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using PressureControllerPtr = std::shared_ptr<interfaces::PressureController>; ///< Alias to
                                                                               ///< PressureController
                                                                               ///< shared pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_PRESSURE_CONTROLLER_HPP