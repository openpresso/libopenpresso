#ifndef HARDWARE_GPIO_PIN_DATA_HPP
#define HARDWARE_GPIO_PIN_DATA_HPP

#include <cstdint>
#include <string>
#include <variant>

#include <libopenpresso/pin_info.hpp>

namespace libopenpresso::gpio
{

using pin_id_t = uint32_t; ///< Unique id that encodes GPIO chip and pin number in one value

/**
 * @enum PinEvent
 * @brief GPIO edge detection events for input pins.
 */
enum class PinEvent : uint8_t {
  RisingEdge = 1 << 0,            ///< Low to high transition
  FallingEdge = 1 << 1,           ///< High to low transition
  Both = RisingEdge | FallingEdge ///< Both rising and falling edges
};

/**
 * @enum PinState
 * @brief GPIO pin state representation.
 */
enum class PinState : uint8_t {
  High, ///< Logical high (typically 3.3V or 5V)
  Low,  ///< Logical low (typically ground)
};

/**
 * @struct PinInfo
 * @brief Base structure for GPIO pin information.
 *
 * Common pin information including address and label.
 */
struct PinInfo {
  PinAddr pinAddr;   ///< GPIO pin address
  std::string label; ///< Used by linux gpio driver, will be shown in gpioinfo (max 32 characters)
};

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
/**
 * @struct InputPinInfo
 * @brief GPIO input pin configuration.
 *
 * Configuration for GPIO pins used as inputs (buttons, sensors, etc.).
 */
struct InputPinInfo : public PinInfo {
  PinPull pinPull;       ///< Pull-up/pull-down configuration
  PinEvent listenEvents; ///< Which edge events to detect
  bool timeCritical;     ///< Each critical pin has a dedicated thread for events processing,
                         ///< non-critical events share one thread
};

/**
 * @struct OutputPinInfo
 * @brief GPIO output pin configuration.
 *
 * Configuration for GPIO pins used as outputs (relays, solenoids, LEDs, etc.).
 */
struct OutputPinInfo : public PinInfo {
  bool initState; ///< Initial pin state on initialization
};
// NOLINTEND(cppcoreguidelines-pro-type-member-init)

using pin_info_t = PinInfo;              ///< Alias for base GPIO pin info
using input_pin_info_t = InputPinInfo;   ///< Alias for GPIO input pin info
using output_pin_info_t = OutputPinInfo; ///< Alias for GPIO output pin info
using any_pin_info_t = std::variant<input_pin_info_t, output_pin_info_t>; ///< Variant for any pin
                                                                          ///< info

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_PIN_DATA_HPP