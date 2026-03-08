/**
 * @file libopenpresso_core.hpp
 * @brief Core component factory and access interface.
 *
 * Defines the LibopenpressoCore interface that serves as the central component
 * factory for an espresso machine control system. All components (sensors,
 * controllers, I/O devices) are accessed through this interface using
 * their unique labels from the DeviceConfig.
 */

#ifndef LIBOPENPRESSO_INTERFACES_LIBOPENPRESSO_CORE_HPP
#define LIBOPENPRESSO_INTERFACES_LIBOPENPRESSO_CORE_HPP

#include <memory>

#include <libopenpresso/interfaces/brew_profiler.hpp>
#include <libopenpresso/interfaces/controller_base.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/logical_input.hpp>
#include <libopenpresso/interfaces/logical_output.hpp>
#include <libopenpresso/interfaces/pressure_controller.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/interfaces/temperature_controller.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

/**
 * @brief Holds interfaces for interacting with top-level library components
 *
 * These interfaces represent the various sensors, controllers, and
 * automation components used in an espresso machine control system.
 * Each component interface is defined in its own header file within
 * the libopenpresso::interfaces namespace.
 */
namespace libopenpresso::interfaces
{

// Forward declarations of all component interfaces
// Removed because they are now included from separate headers to provide shared pointer aliases

/**
 * @class LibopenpressoCore
 * @brief Abstract component factory interface for libopenpresso components.
 *
 * LibopenpressoCore is the central component access point that implements a lazy
 * factory pattern. All top-level components (those who implement an interface
 * from libopenpresso::interfaces) are created on-demand when first accessed and
 * cached for subsequent requests.
 *
 * ### Component Access:
 * - Accept a @p label unique identifier (string) from DeviceConfig
 * - Return std::shared_ptr<> to the component interface
 * - Create the component on first access, cache for future accesses
 * - Throw libopenpresso::Exception if label not found or config type mismatch
 *
 * @code
 * auto core = libopenpresso::getCore(config);
 * auto pressure = core->getPressureSensor("group_pressure");
 * auto pump = core->getPressureController("pump_ctrl");
 * @endcode
 *
 * ### Lifetime Management:
 *
 * - All the components ever requested will remain alive at least as long as LibopenpressoCore is
 * alive
 * - At the same time, components are self-contained and do not depend on LibopenpressoCore after
 * creation
 * - User should take care about object lifetimes that are referenced inside registered callbacks
 *
 * @see libopenpresso::DeviceConfig, libopenpresso::getCore()
 * WeightSensor, PressureSensor, TemperatureSensor
 * PressureController, TemperatureController, FlowRateController
 * LogicalInput, LogicalOutput, BrewProfiler, ControllerBase, libopenpresso::interfaces
 */

/**
 * @fn virtual std::shared_ptr<WeightSensor> LibopenpressoCore::getWeightSensor(const
 * component_label_t& label)
 * @brief Retrieves or creates the weight sensor component with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref WeightSensor> Shared pointer to the weight sensor interface.
 *         Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a WeightSensor
 *         - Hardware initialization fails (I2C errors, etc.)
 *
 * @details Weight sensors measure:
 *          - Espresso dose weight in the cup on the drip tray
 *          - Water weight pumped from the reservoir (rough estimate using pump pulse counting)
 *          - Flow rate (derived from weight change over time)
 *
 * @note Supported implementation configs: Nau7802WeightSensorConfig, VirtualWeightSensorConfig
 */

/**
 * @fn virtual std::shared_ptr<FlowRateController> LibopenpressoCore::getFlowRateController(const
 * component_label_t& label)
 * @brief Retrieves or creates the flow rate controller with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref FlowRateController> Shared pointer to the flow rate controller
 * interface. Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a FlowRateController
 *         - Hardware initialization fails
 *
 * @details Flow rate controllers regulate water flow by modulating:
 *          - Pump pulse frequency
 *          - Solenoid valve opening
 *          - Proportional valve position
 *
 * @note Supported implementation configs: VibroPumpFlowController, IntegralFlowRateControllerConfig
 */

/**
 * @fn virtual std::shared_ptr<PressureSensor> LibopenpressoCore::getPressureSensor(const
 * component_label_t& label)
 * @brief Retrieves or creates the pressure sensor with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref PressureSensor> Shared pointer to the pressure sensor interface.
 *         Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a PressureSensor
 *         - Hardware initialization fails (I2C/ADC errors, etc.)
 *
 * @details Pressure sensors measure:
 *          - Group head brewing pressure
 *          - Boiler pressure (steam or brew)
 *          - Pump outlet pipe pressure
 *
 * @note Supported implementation configs: Ads1115PressureSensorConfig
 */

/**
 * @fn virtual std::shared_ptr<PressureController> LibopenpressoCore::getPressureController(const
 * component_label_t& label)
 * @brief Retrieves or creates the pressure controller with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref PressureController> Shared pointer to the pressure controller
 * interface. Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a PressureController
 *         - Hardware initialization fails
 *
 * @details Pressure controllers regulate system pressure by:
 *          - Modulating pump speed
 *          - Opening/closing solenoid valves
 *          - Adjusting proportional valve position
 *
 *          Uses pressure sensor readings feedback-loop control.
 *
 * @note Supported implementation configs: PulsePressureControllerConfig
 */

/**
 * @fn virtual std::shared_ptr<TemperatureSensor> LibopenpressoCore::getTemperatureSensor(const
 * component_label_t& label)
 * @brief Retrieves or creates the temperature sensor with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref TemperatureSensor> Shared pointer to the temperature sensor
 * interface. Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a TemperatureSensor
 *         - Hardware initialization fails (SPI/I2C errors, etc.)
 *
 * @details Temperature sensors measure:
 *          - Brew water temperature
 *          - Steam boiler temperature
 *          - Group head temperature
 *          - Other thermal zones
 *
 * @note Supported implementation configs: Max6675TemperatureSensorConfig,
 * Max31856TemperatureSensorConfig
 */

/**
 * @fn virtual std::shared_ptr<TemperatureController> LibopenpressoCore::getTemperatureController(
 * const component_label_t& label)
 * @brief Retrieves or creates the temperature controller with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref TemperatureController> Shared pointer to the temperature controller
 * interface. Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a TemperatureController
 *         - Hardware initialization fails
 *
 * @details Temperature controllers maintain thermal stability by:
 *          - Modulating heater elements (via PWM or PDM with SSR or triac)
 *          - PID + feed forward control algorithms
 *
 *          Uses temperature sensor readings feedback-loop control.
 *
 * @note Supported implementation configs: TemperaturePidControllerConfig
 */

/**
 * @fn virtual std::shared_ptr<LogicalOutput> LibopenpressoCore::getLogicalOutput(const
 * component_label_t& label)
 * @brief Retrieves or creates the logical output (actuator) with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref LogicalOutput> Shared pointer to the logical output interface.
 *         Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a LogicalOutput
 *         - Hardware initialization fails (GPIO errors, etc.)
 *
 * @details Logical outputs control binary on/off devices:
 *          - Solenoid valves (3-way for group head)
 *          - Relays for switching higher currents
 *          - Pump enable/disable
 *          - LED indicators
 *
 * @note Supported implementation configs: LogicalOutputPinConfig
 */

/**
 * @fn virtual std::shared_ptr<LogicalInput> LibopenpressoCore::getLogicalInput(const
 * component_label_t& label)
 * @brief Retrieves or creates the logical input (sensor/button) with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref LogicalInput> Shared pointer to the logical input interface.
 *         Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a LogicalInput
 *         - Hardware initialization fails (GPIO errors, etc.)
 *
 * @details Logical inputs monitor binary state changes from:
 *          - Front panel buttons (power, brew, etc.)
 *          - Limit switches (water level, door position)
 *          - AC mains zero-crossing detection
 *
 *          Provides debounced state with event callbacks.
 *
 * @note Supported implementation configs: LogicalInputPinConfig, AcZeroCrossSensorConfig
 */

/**
 * @fn virtual std::shared_ptr<BrewProfiler> LibopenpressoCore::getBrewProfiler(const
 * component_label_t& label)
 * @brief Retrieves or creates the brew profiler with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref BrewProfiler> Shared pointer to the brew profiler interface.
 *         Subsequent calls with the same label return the cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a BrewProfiler
 *         - Dependency component initialization fails
 *
 * @details The brew profiler automates espresso extraction by:
 *          - Executing predefined multi-step pressure/flow profiles
 *          - Transitioning steps based on weight or time
 *          - Notifying clients of step changes via callbacks
 *
 *          Enables reproducible extraction profiles for consistent quality.
 *
 * @note Supported implementation configs: BrewProfilerConfig
 *
 * @see BrewProfiler, BrewProfilerConfig, brew_steps_data.hpp
 */

/**
 * @fn virtual std::shared_ptr<ControllerBase> LibopenpressoCore::getSteamController(const
 * component_label_t& label)
 * @brief Retrieves or creates the steam controller with the specified label.
 *
 * @param[in] label Unique component identifier from DeviceConfig.
 *
 * @return std::shared_ptr<@ref ControllerBase> Shared pointer to the steam controller.
 *         Returned as generic ControllerBase since steam controller has no specific methods
 *         beyond activation control. Subsequent calls with the same label return cached instance.
 *
 * @throws libopenpresso::Exception if:
 *         - Label not found in DeviceConfig
 *         - Config for this label cannot be used to create a steam controller
 *         - Dependency component initialization fails
 *
 * @details The steam controller manages:
 *          - Steam boiler heating to target temperature
 *          - Boiler pressure regulation
 *          - Water refill logic and flow control
 *          - Pressure/temperature interlocks for safety
 *          Coordinates multiple sub-controllers for complete steam system operation.
 *
 * @note Supported implementation configs: SteamControllerConfig
 */

/**
 * @fn virtual LibopenpressoCore::~LibopenpressoCore()
 * @brief Virtual destructor for polymorphic cleanup.
 *
 * Ensures all component resources are properly released when the Core is destroyed.
 * Derived classes should implement proper cleanup of internal threads and hardware.
 */

class LibopenpressoCore {
public:
  virtual WeightSensorPtr getWeightSensor(const component_label_t& label) = 0;
  virtual FlowRateControllerPtr getFlowRateController(const component_label_t& label) = 0;
  virtual PressureSensorPtr getPressureSensor(const component_label_t& label) = 0;
  virtual PressureControllerPtr getPressureController(const component_label_t& label) = 0;
  virtual TemperatureSensorPtr getTemperatureSensor(const component_label_t& label) = 0;
  virtual TemperatureControllerPtr getTemperatureController(const component_label_t& label) = 0;
  virtual LogicalOutputPtr getLogicalOutput(const component_label_t& label) = 0;
  virtual LogicalInputPtr getLogicalInput(const component_label_t& label) = 0;
  virtual BrewProfilerPtr getBrewProfiler(const component_label_t& label) = 0;
  virtual ControllerBasePtr getSteamController(const component_label_t& label) = 0;

  virtual ~LibopenpressoCore() = default;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using CorePtr = std::shared_ptr<interfaces::LibopenpressoCore>; ///< Alias to LibopenpressoCore ///<
                                                                ///< shared pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_LIBOPENPRESSO_CORE_HPP
