/**
 * @file config.hpp
 * @brief Device configuration types and structures.
 *
 * Defines the DeviceConfig structure and component configuration variant
 * that serves as the blueprint for creating an libopenpresso Core instance.
 */

#ifndef LIBOPENPRESSO_CONFIG_HPP
#define LIBOPENPRESSO_CONFIG_HPP

#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/types.hpp>

namespace spdlog
{
class logger;
} // namespace spdlog

namespace libopenpresso
{

/**
 * @typedef component_config_t
 * @brief Variant type holding all supported component configuration types.
 *
 * Serves for storing different config types into DeviceConfig components map
 */
using component_config_t = std::variant<Ads1115PressureSensorConfig,
                                        AcZeroCrossSensorConfig,
                                        PulseControlledDeviceConfig,
                                        PulsePowerControllerConfig,
                                        VibroPumpFlowSensor,
                                        VibroPumpFlowController,
                                        VirtualWeightSensorConfig,
                                        IntegralFlowRateControllerConfig,
                                        PulsePressureControllerConfig,
                                        Max6675TemperatureSensorConfig,
                                        Max31856TemperatureSensorConfig,
                                        TemperaturePidControllerConfig,
                                        LogicalInputPinConfig,
                                        LogicalOutputPinConfig,
                                        BrewProfilerConfig,
                                        SteamControllerConfig,
                                        Nau7802WeightSensorConfig>;

/**
 * @typedef components_map_t
 * @brief Map of component configurations indexed by unique labels.
 *
 * Each component in the system is identified by a unique string label
 * that can be used to:
 * - Look up the component's configuration
 * - Reference the component as a dependency in other component configs
 * - Retrieve the component from the LibopenpressoCore interface
 */
using components_map_t = std::unordered_map<component_label_t, component_config_t>;

/**
 * @struct DeviceConfig
 * @brief Complete device configuration for creating an libopenpresso Core.
 *
 * Contains all information needed to initialize a complete espresso machine
 * control system: component definitions, inter-dependencies, and global settings.
 *
 * ### Usage:
 * @code
 * DeviceConfig config;
 * config.components["pressure_sensor"] = Ads1115PressureSensorConfig{...};
 * config.components["temp_sensor"] = Max6675TemperatureSensorConfig{...};
 * config.watchdog = WatchdogConfig{...};
 *
 * auto core = libopenpresso::getCore(config);
 * @endcode
 *
 * ### Components dependencies:
 * Many components depend on other components. Dependencies are expressed by
 * including the label of the dependency in the component's config struct.
 * The Core resolves these during component creation.
 *
 * Example dependency chain:
 * @code
 * // TemperaturePidControllerConfig depends on a sensor and power controller
 * TemperaturePidControllerConfig tc{
 *     .powerController = "heater_pdm",      // reference to another component
 *     .sensor = "boiler_temp",              // reference to another component
 *     .pidSettings = {...},
 *     ...
 * };
 * @endcode
 *
 * @see DeviceConfig::components, DeviceConfig::watchdog
 * @see libopenpresso::getCore(), component_config.hpp
 */
struct DeviceConfig {
  /**
   * @brief All component configurations indexed by their unique labels.
   *
   * Maps each component's label to its configuration struct.
   * The Core uses these configs to create component instances on demand
   * and cache them for subsequent accesses.
   */
  components_map_t components;

  /**
   * @brief System watchdog configuration (optional).
   *
   * If present, initializes a watchdog timer that monitors system health
   * and triggers hardware reset if health check fails.
   *
   * Watchdog triggers reset in scenarios like:
   * - **Application hang**: If watchdog is not kicked within timeout period
   * - **Sensor malfunction**: If sensor stops sending values or values run out of allowed range
   * - **GPIO failures**: If changing output pin state fails
   * - **Uncaught exceptions**: Application exits abnormally
   *
   * The watchdog is a global component (doesn't require a label) since
   * the system only needs one main watchdog timer.
   *
   * @note If std::nullopt, watchdog is disabled. This is not recommended
   *       for production use, but is necessary for debugging.
   *
   * @see WatchdogConfig
   */
  std::optional<WatchdogConfig> watchdog;

  /**
   * @brief Libopenpresso internal logging object
   *
   * Libopenpresso uses [spdlog](https://github.com/gabime/spdlog)
   * as its logging library. Set to spdlog::default_logger() for
   * basic console output or any customized logger you want.
   * If not set, no logs will be emitted.
   */
  std::shared_ptr<spdlog::logger> logger;
};

} // namespace libopenpresso

#endif // LIBOPENPRESSO_CONFIG_HPP