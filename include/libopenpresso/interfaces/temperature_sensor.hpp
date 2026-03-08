/**
 * @file temperature_sensor.hpp
 * @brief Interface for temperature measurement devices.
 *
 * Defines the interface for all temperature sensors used in espresso machines
 * (brew boiler, steam boiler, group head, etc.).
 */

#ifndef LIBOPENPRESSO_INTERFACES_TEMPERATURE_SENSOR_HPP
#define LIBOPENPRESSO_INTERFACES_TEMPERATURE_SENSOR_HPP

#include <functional>
#include <memory>
#include <optional>

#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @class TemperatureSensor
 * @brief Interface for temperature measurement with callback support.
 *
 * Provides temperature value readings and optional callback notifications
 * when measurements are updated.
 *
 * @note Supported implementation configs: Max6675TemperatureSensorConfig,
 * Max31856TemperatureSensorConfig
 */

/**
 * @typedef TemperatureSensor::callback_t
 * @brief Callback function type for temperature updates.
 *
 * Called with the current temperature value in millidegrees Celsius.
 */

/**
 * @fn virtual millidegrees_t TemperatureSensor::getTemperature() const
 * @brief Get the current temperature reading.
 *
 * @return Current temperature in millidegrees Celsius (thousandths of a degree).
 *         For example, 85500 represents 85.5°C.
 */

/**
 * @fn virtual std::optional<time_delta_t> TemperatureSensor::fixedUpdateRate() const noexcept
 * @brief Callbacks fixed update rate.
 *
 * Indicates whether the registered callbacks are called at regular intervals.
 *
 * @return std::optional<time_delta_t> The notification period if fixed, std::nullopt otherwise.
 *
 * @see PressureSensor::fixedUpdateRate()
 */

/**
 * @fn virtual callback_descriptor_t TemperatureSensor::registerCallback(const callback_t&)
 * @brief Register a callback for temperature updates.
 *
 * Whenever a new temperature measurement is available, the registered callback
 * is invoked with the measured value.
 *
 * @param[in] callback Function to invoke on measurement updates.
 * @return callback_descriptor_t Descriptor for later unsubscribing from callbacks.
 *
 * @see unregisterCallback()
 */

/**
 * @fn virtual void TemperatureSensor::unregisterCallback(callback_descriptor_t)
 * @brief Unregister a previously registered callback.
 *
 * @param[in] descriptor Descriptor returned from registerCallback().
 *
 * @see registerCallback()
 */

/**
 * @fn virtual TemperatureSensor::~TemperatureSensor()
 * @brief Virtual destructor for polymorphic cleanup.
 */

class TemperatureSensor {
public:
  using callback_t = std::function<void(millidegrees_t)>;

  virtual millidegrees_t getTemperature() const = 0;
  virtual std::optional<time_delta_t> fixedUpdateRate() const noexcept = 0;
  virtual callback_descriptor_t registerCallback(const callback_t& callback) = 0;

  /**
   * @brief Unregister a previously registered callback.
   *
   * @param[in] descriptor Descriptor returned from registerCallback().
   *
   * @see registerCallback()
   */
  virtual void unregisterCallback(callback_descriptor_t descriptor) = 0;
  /**
   * @brief Virtual destructor for polymorphic cleanup.
   */
  virtual ~TemperatureSensor() = default;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using TemperatureSensorPtr = std::shared_ptr<interfaces::TemperatureSensor>; ///< Alias to
                                                                             ///< TemperatureSensor
                                                                             ///< shared pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_TEMPERATURE_SENSOR_HPP