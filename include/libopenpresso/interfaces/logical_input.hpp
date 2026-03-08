/**
 * @file logical_input.hpp
 * @brief Interface for logical input devices (buttons, sensors, etc.).
 *
 * Defines the interface for logical input devices that report binary state changes
 * with optional event-driven callbacks.
 */

#ifndef LIBOPENPRESSO_INTERFACES_LOGICAL_INPUT_HPP
#define LIBOPENPRESSO_INTERFACES_LOGICAL_INPUT_HPP

#include <functional>
#include <memory>
#include <optional>

#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @class LogicalInput
 * @brief Interface for digital input with callback support.
 *
 * Represents binary input devices like buttons, switches, or digital sensors
 * that report their state (high/low) with optional callback notifications.
 *
 * @note Supported implementation configs: LogicalInputPinConfig, AcZeroCrossSensorConfig
 */

/**
 * @typedef LogicalInput::callback_t
 * @brief Callback function type for state changes.
 *
 * Called when the input state changes, with the new state (true/false).
 */

/**
 * @fn virtual bool LogicalInput::getState() const
 * @brief Get the current input state.
 *
 * @return true for high/pressed, false for low/released
 * (inversion can be added to the component config if supported).
 */

/**
 * @fn virtual std::optional<time_delta_t> LogicalInput::fixedUpdateRate() const noexcept
 * @brief Callbacks fixed update rate.
 *
 * Indicates whether the registered callbacks are called at regular intervals.
 *
 * @return std::optional<time_delta_t> The notification period if fixed, std::nullopt otherwise.
 */

/**
 * @fn virtual callback_descriptor_t LogicalInput::registerCallback(const callback_t&)
 * @brief Register a callback for state change events.
 *
 * Whenever the input state changes (after debouncing), the callback
 * is invoked with the new state.
 *
 * @param[in] callback Function to invoke on state changes.
 * @return callback_descriptor_t Descriptor for later unsubscribing from callbacks.
 *
 * @see unregisterCallback()
 */

/**
 * @fn virtual void LogicalInput::unregisterCallback(callback_descriptor_t)
 * @brief Unregister a previously registered callback.
 *
 * @param[in] descriptor Descriptor returned from registerCallback().
 *
 * @see registerCallback()
 */

class LogicalInput {
public:
  using callback_t = std::function<void(bool)>;

  virtual bool getState() const = 0;

  virtual std::optional<time_delta_t> fixedUpdateRate() const noexcept = 0;

  virtual callback_descriptor_t registerCallback(const callback_t& callback) = 0;

  virtual void unregisterCallback(callback_descriptor_t descriptor) = 0;

  virtual ~LogicalInput() = default;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using LogicalInputPtr = std::shared_ptr<interfaces::LogicalInput>; ///< Alias to LogicalInput shared
                                                                   ///< pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_LOGICAL_INPUT_HPP