/**
 * @file logical_output.hpp
 * @brief Interface for logical output devices (solenoids, relays, LEDs, etc.).
 *
 * Defines the interface for digital output devices that can be switched on/off
 * to control actuators and indicators.
 */

#ifndef LIBOPENPRESSO_INTERFACES_LOGICAL_OUTPUT_HPP
#define LIBOPENPRESSO_INTERFACES_LOGICAL_OUTPUT_HPP

#include <memory>

#include <libopenpresso/interfaces/controller_base.hpp>

namespace libopenpresso::interfaces
{

/**
 * @fn virtual bool LogicalOutput::getState() const
 * @brief Get the current output state.
 *
 * @return true if output is high/activated, false if low/deactivated.
 */

/**
 * @fn virtual void LogicalOutput::setState(bool)
 * @brief Set the output state.
 *
 * @param[in] state true to activate (high), false to deactivate (low).
 */

/**
 * @class LogicalOutput
 * @brief Interface for digital output control (on/off actuators).
 *
 * Represents binary output devices like solenoid valves, relays, pumps, or LEDs
 * that can be switched between two states. Inherits activation control from
 * ControllerBase.
 *
 * @note Supported implementation configs: LogicalOutputPinConfig
 *
 * @see ControllerBase
 */
class LogicalOutput : public ControllerBase {
public:
  virtual bool getState() const = 0;

  virtual void setState(bool state) = 0;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using LogicalOutputPtr = std::shared_ptr<interfaces::LogicalOutput>; ///< Alias to LogicalOutput
                                                                     ///< shared pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_LOGICAL_OUTPUT_HPP