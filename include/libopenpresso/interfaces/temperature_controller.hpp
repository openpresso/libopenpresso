/**
 * @file temperature_controller.hpp
 * @brief Interface for temperature regulation controllers.
 *
 * Defines the interface for controllers that regulate system temperature
 * (brew boiler, steam boiler, or group head).
 */

#ifndef LIBOPENPRESSO_INTERFACES_TEMPERATURE_CONTROLLER_HPP
#define LIBOPENPRESSO_INTERFACES_TEMPERATURE_CONTROLLER_HPP

#include <memory>

#include <libopenpresso/interfaces/controller_base.hpp>
#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @class TemperatureController
 * @brief Interface for temperature regulation with setpoint control.
 *
 * Controls system temperature toward a target setpoint by modulating
 * heating elements or actuators. Inherits activation control from ControllerBase.
 *
 * @note Supported implementation configs: TemperaturePidControllerConfig
 *
 * @see ControllerBase
 */

/**
 * @fn virtual millidegrees_t TemperatureController::getTargetTemperature() const
 * @brief Get the current temperature target setpoint.
 *
 * @return Current target temperature in millidegrees Celsius.
 */

/**
 * @fn virtual void TemperatureController::setTargetTemperature(millidegrees_t)
 * @brief Set the temperature target setpoint.
 *
 * When the controller is active, it will regulate temperature toward this value.
 *
 * @param[in] millidegrees Target temperature in millidegrees Celsius.
 */

class TemperatureController : public ControllerBase {
public:
  virtual millidegrees_t getTargetTemperature() const = 0;
  virtual void setTargetTemperature(millidegrees_t millidegrees) = 0;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using TemperatureControllerPtr = std::shared_ptr<interfaces::TemperatureController>; ///< Alias to
                                                                                     ///< TemperatureController
                                                                                     ///< shared
                                                                                     ///< pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_TEMPERATURE_CONTROLLER_HPP