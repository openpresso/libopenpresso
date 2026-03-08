/**
 * @file weight_sensor.hpp
 * @brief Interface for weight measurement and flow rate estimation.
 *
 * Defines the interface for weight sensors used to measure espresso dose weight
 * and derived flow rate during brewing.
 */

#ifndef LIBOPENPRESSO_INTERFACES_WEIGHT_SENSOR_HPP
#define LIBOPENPRESSO_INTERFACES_WEIGHT_SENSOR_HPP

#include <functional>
#include <memory>
#include <optional>

#include <libopenpresso/types.hpp>

namespace libopenpresso::interfaces
{

/**
 * @class WeightSensor
 * @brief Interface for weight measurement with callback support.
 *
 * Provides weight value readings, flow rate calculation, tare functionality,
 * and callback notifications for measurement updates.
 *
 * @note Supported implementation configs: Nau7802WeightSensorConfig, VirtualWeightSensorConfig
 */

/**
 * @typedef WeightSensor::callback_t
 * @brief Callback function type for weight and flow rate updates.
 *
 * Called with current weight and calculated flow rate.
 */

/**
 * @fn virtual void WeightSensor::tare()
 * @brief Reset the scale to zero (tare operation).
 *
 * Sets the current measurement as the new zero reference point,
 * commonly used before brewing to account for cup and portafilter weight.
 */

/**
 * @fn virtual milligrams_t WeightSensor::getWeight() const
 * @brief Get the current weight measurement.
 *
 * @return Current weight in milligrams relative to tare point.
 * @note Negative values indicate weight below tare.
 */

/**
 * @fn virtual milligrams_p_second_t WeightSensor::getFlowRate() const
 * @brief Get the current flow rate (derived from weight change).
 *
 * Calculated as the rate of weight change over time, typically smoothed.
 *
 * @return Current flow rate in milligrams per second.
 * @note Negative values indicate decreasing weight (e.g., removing object from scales).
 */

/**
 * @fn virtual std::optional<time_delta_t> WeightSensor::fixedUpdateRate() const noexcept
 * @brief Callbacks fixed update rate.
 *
 * Indicates whether the registered callbacks are called at regular intervals.
 *
 * @return std::optional<time_delta_t> The notification period if fixed, std::nullopt otherwise.
 *
 * @see PressureSensor::fixedUpdateRate()
 */

/**
 * @fn virtual callback_descriptor_t WeightSensor::registerCallback(const callback_t&)
 * @brief Register a callback for measurement updates.
 *
 * Callback receives both weight and flow rate with each update.
 *
 * @param[in] callback Function to invoke on measurement updates.
 * @return callback_descriptor_t Descriptor for later unsubscribing from callbacks.
 *
 * @see unregisterCallback()
 */

/**
 * @fn virtual void WeightSensor::unregisterCallback(callback_descriptor_t)
 * @brief Unregister a previously registered callback.
 *
 * @param[in] descriptor Descriptor returned from registerCallback().
 *
 * @see registerCallback()
 */

/**
 * @fn virtual WeightSensor::~WeightSensor()
 * @brief Virtual destructor for polymorphic cleanup.
 */

class WeightSensor {
public:
  using callback_t = std::function<void(milligrams_t, milligrams_p_second_t)>;

  virtual void tare() = 0;
  virtual milligrams_t getWeight() const = 0;
  virtual milligrams_p_second_t getFlowRate() const = 0;
  virtual std::optional<time_delta_t> fixedUpdateRate() const noexcept = 0;
  virtual callback_descriptor_t registerCallback(const callback_t& callback) = 0;
  virtual void unregisterCallback(callback_descriptor_t descriptor) = 0;
  /**
   * @brief Virtual destructor for polymorphic cleanup.
   */
  virtual ~WeightSensor() = default;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using WeightSensorPtr = std::shared_ptr<interfaces::WeightSensor>; ///< Alias to WeightSensor shared
                                                                   ///< pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_WEIGHT_SENSOR_HPP
