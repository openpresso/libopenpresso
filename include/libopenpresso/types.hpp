/**
 * @file types.hpp
 * @brief Type definitions and aliases for espresso machine control.
 *
 * Core type definitions for physical measurements, configuration IDs,
 * and system control values used throughout the openpresso library.
 */
#ifndef LIBOPENPRESSO_TYPES_HPP
#define LIBOPENPRESSO_TYPES_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>

namespace libopenpresso
{

using pid_coeffs_t = float;           ///< Type used for PID controller tune settings.
using callback_descriptor_t = size_t; ///< Descriptor of the registered callback, serves for
                                      ///< unregistering this callback when needed.
using millibars_t = uint32_t;         ///< Pressure units type
using millidegrees_t = int32_t;       ///< Temperature units type. Celsius assumed.
using micrograms_t = uint32_t; ///< Weight units type used in flow sensors where milligrams could be
                               ///< not enough
using milligrams_t = int32_t;  ///< Weight units type used everywhere except flow sensors
using milligrams_p_second_t = int32_t;         ///< Flow rate units type
using pin_number_t = uint16_t;                 ///< GPIO BCM pin number
using i2c_dev_addr_t = uint8_t;                ///< 7-bit I2C device address
using unix_dev_addr_t = std::filesystem::path; ///< Path to the unix character device, normally
                                               ///< starts with /dev
using pid_calc_t = float;      ///< Type used for PID controller internal calculations
using power_units_t = uint8_t; ///< Units are meaningless, value representing 100% power is
                               ///< signalled by openpresso::gpio::PowerController::powerMax()
using time_delta_t = std::chrono::steady_clock::duration; ///< Type for representing time intervals
using component_label_t = std::string; ///< Type for component labels in the DeviceConfig

} // namespace libopenpresso

#endif // LIBOPENPRESSO_TYPES_HPP