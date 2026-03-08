/**
 * @file i2c_info.hpp
 * @brief I2C bus addressing and configuration structures.
 *
 * Defines I2C device addressing for sensors and peripherals connected
 * via I2C bus communication.
 */

#ifndef LIBOPENPRESSO_I2C_INFO_HPP
#define LIBOPENPRESSO_I2C_INFO_HPP

#include <libopenpresso/types.hpp>

namespace libopenpresso
{

/**
 * @struct I2cAddr
 * @brief I2C device address combining bus and device address.
 *
 * Uniquely identifies an I2C peripheral by its bus and 7-bit device address.
 */
struct I2cAddr {
  unix_dev_addr_t bus; ///< Path to I2C bus device (e.g., /dev/i2c-1)
  i2c_dev_addr_t dev;  ///< 7-bit I2C device address
};

/**
 * @brief alias for I2C device address
 */
using i2c_addr_t = I2cAddr;

} // namespace libopenpresso

#endif // LIBOPENPRESSO_I2C_INFO_HPP