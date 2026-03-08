/**
 * @file pin_info.hpp
 * @brief GPIO pin configuration and information structures.
 *
 * Defines GPIO pin addressing, state representation, and configuration structures
 * for both input and output pins used in espresso machine control.
 */

#ifndef LIBOPENPRESSO_PIN_INFO_HPP
#define LIBOPENPRESSO_PIN_INFO_HPP

#include <cstdint>

#include <libopenpresso/types.hpp>

namespace libopenpresso
{

/**
 * @enum PinPull
 * @brief GPIO pull-up/pull-down configuration for input pins.
 */
enum class PinPull : uint8_t {
  No,       ///< No pull resistor
  PullUp,   ///< Pull-up resistor enabled
  PullDown, ///< Pull-down resistor enabled
};

/**
 * @struct PinAddr
 * @brief GPIO pin address using character device and pin number.
 *
 * Identifies a GPIO pin by the character device (typically /dev/gpiochipX) and
 * the pin number relative to that GPIO controller.
 */

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
struct PinAddr {
  unix_dev_addr_t chip; ///< Path to GPIO character device (e.g., /dev/gpiochip0)
  pin_number_t pin;     ///< Pin number on the GPIO controller
};
// NOLINTEND(cppcoreguidelines-pro-type-member-init)

using pin_addr_t = PinAddr; ///< Alias for GPIO pin address

} // namespace libopenpresso

#endif // LIBOPENPRESSO_PIN_INFO_HPP