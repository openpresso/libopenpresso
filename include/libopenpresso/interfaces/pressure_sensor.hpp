/**
 * @file pressure_sensor.hpp
 * @brief Interface for pressure measurement devices.
 *
 * Defines the interface for all pressure sensors used in espresso machines
 * (group head, boiler, pump outlet pipe, etc.).
 */

#ifndef LIBOPENPRESSO_INTERFACES_PRESSURE_SENSOR_HPP
#define LIBOPENPRESSO_INTERFACES_PRESSURE_SENSOR_HPP

#include <functional>
#include <memory>
#include <optional>

#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @class PressureSensor
 * @brief Interface for pressure measurement with callback support.
 *
 * Provides pressure value readings and optional callback notifications
 * when measurements are updated.
 *
 * @note Supported implementation configs: Ads1115PressureSensorConfig
 */

/**
 * @typedef PressureSensor::callback_t
 * @brief Callback function type for pressure updates.
 *
 * Called with the current pressure value in millibars.
 */

/**
 * @fn virtual millibars_t PressureSensor::getPressure() const
 * @brief Get the current pressure reading.
 *
 * @return Current pressure in millibars (thousandths of a bar).
 */

/**
 * @fn virtual std::optional<time_delta_t> PressureSensor::fixedUpdateRate() const noexcept
 * @brief Callbacks fixed update rate.
 *
 * Indicates whether the registered callbacks are called at regular intervals.
 *
 * @return std::optional<time_delta_t> The notification period if fixed, std::nullopt otherwise.
 */

/**
 * @fn virtual callback_descriptor_t PressureSensor::registerCallback(const callback_t&)
 * @brief Register a callback for pressure updates.
 *
 * Whenever a new pressure measurement is available, the registered callback
 * is invoked with the measured value.
 *
 * @param[in] callback Function to invoke on measurement updates.
 * @return callback_descriptor_t Descriptor for later unsubscribing from callbacks.
 *
 * @see unregisterCallback()
 */

/**
 * @fn virtual void PressureSensor::unregisterCallback(callback_descriptor_t)
 * @brief Unregister a previously registered callback.
 *
 * @param[in] descriptor Descriptor returned from registerCallback().
 *
 * @see registerCallback()
 */

/**
 * @fn virtual PressureSensor::~PressureSensor()
 * @brief Virtual destructor for polymorphic cleanup.
 */

class PressureSensor {
public:
  using callback_t = std::function<void(millibars_t)>;

  virtual millibars_t getPressure() const = 0;
  virtual std::optional<time_delta_t> fixedUpdateRate() const noexcept = 0;
  virtual callback_descriptor_t registerCallback(const callback_t& callback) = 0;
  virtual void unregisterCallback(callback_descriptor_t descriptor) = 0;
  virtual ~PressureSensor() = default;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using PressureSensorPtr = std::shared_ptr<interfaces::PressureSensor>; ///< Alias to PressureSensor
                                                                       ///< shared pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_PRESSURE_SENSOR_HPP