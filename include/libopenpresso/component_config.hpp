/**
 * @file component_config.hpp
 * @brief Configuration structures for all espresso machine components.
 *
 * Defines configuration data structures for sensors, controllers, and peripherals
 * used in the espresso machine control system. Each config struct specifies the
 * parameters and dependencies needed to create and initialize a specific component.
 */

#ifndef LIBOPENPRESSO_COMPONENT_CONFIG_HPP
#define LIBOPENPRESSO_COMPONENT_CONFIG_HPP

#include <cstddef>
#include <cstdint>
#include <optional>

#include <libopenpresso/i2c_info.hpp>
#include <libopenpresso/pid_settings.hpp>
#include <libopenpresso/pin_info.hpp>
#include <libopenpresso/types.hpp>

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
namespace libopenpresso
{

/**
 * @struct Ads1115PressureSensorConfig
 * @brief Configuration for ADS1115 I2C analog pressure sensor.
 *
 * The ADS1115 is a 16-bit ADC that reads analog voltage level
 * from a pressure transducer.
 *
 * @note Call interfaces::LibopenpressoCore::getPressureSensor()
 *       with the label of this config to get the sensor instance.
 */
struct Ads1115PressureSensorConfig {
  i2c_addr_t addr;      ///< I2C address of the ADS1115 device
  pin_addr_t signalPin; ///< GPIO pin for data ready signal
};

/**
 * @struct AcZeroCrossSensorConfig
 * @brief Configuration for AC zero-crossing detection sensor.
 *
 * This sensor component detects AC mains zero-crossing for correct
 * operation of pulse-controlled devices and synching of the pulse
 * density modulation output to AC phase.
 *
 * @note Call interfaces::LibopenpressoCore::getLogicalInput()
 *       with the label of this config to get configured component instance.
 */
struct AcZeroCrossSensorConfig {
  pin_addr_t signalPin; ///< GPIO pin receiving the zero-crossing signal
};

/**
 * @struct PulseControlledDeviceConfig
 * @brief Configuration for pulse-controlled AC device (heater, vibro-pump).
 *
 * Controls AC devices using zero-crossing synchronized switching.
 * When new AC sine phase starts, it checks should the power be cut from
 * the controlled device or not with a callback.
 *
 * @note This component isn't exposed directly, but used as a dependency by other components.
 *
 * @see AcZeroCrossSensorConfig
 */
struct PulseControlledDeviceConfig {
  pin_addr_t pulsePin;                 ///< GPIO pin for pulse control output
  component_label_t acZeroCrossSensor; ///< Dependency component label, should point to
                                       ///< AcZeroCrossSensorConfig
};

/**
 * @struct PulsePowerControllerConfig
 * @brief Configuration for pulse density modulation power controller.
 *
 * Controls power output to devices using PDM with configurable duty cycle.
 * The more duty cycle is, the more precise power control is possible,
 * but less responsive the control becomes. For heaters, typical values
 * are ~50 cycles (1 second at 50Hz mains), for vibro-pumps ~10 cycles.
 *
 * @note This component isn't exposed directly, but used as a dependency by other components.
 *
 * @see PulseControlledDeviceConfig
 */
struct PulsePowerControllerConfig {
  component_label_t pulseControlledDevice; ///< Dependency component label, should point to
                                           ///< PulseControlledDeviceConfig
  size_t dutyCycle;                        ///< Duty cycle in AC mains cycles
};

/**
 * @struct VibroPumpFlowSensor
 * @brief Configuration for vibro pump flow sensor using pressure-based measurement.
 *
 * Sends callbacks to other components with rough estimation of amount of water pumped since the
 * last callback.
 *
 * The amount of water pumped is calculated as:
 * \f[ \text{water} = \left(1 - \min\left(\frac{P}{P_{\text{stall}}}, 1\right)\right) \times
 * V_{\text{pulse}} \times N_{\text{pulses}} \f]
 *
 * where:
 * - \f$P\f$ is the current pressure (in millibars)
 * - \f$P_{\text{stall}}\f$ is the VibroPumpFlowSensor::pumpStallPressure
 * - \f$V_{\text{pulse}}\f$ is the VibroPumpFlowSensor::volumePerPulse
 * - \f$N_{\text{pulses}}\f$ is the number of pump pulses since the last callback
 *
 * @note This component isn't exposed directly, but used as a dependency by other components.
 *
 * @see Ads1115PressureSensorConfig, PulseControlledDeviceConfig
 */
struct VibroPumpFlowSensor {
  component_label_t pressureSensor; ///< Dependency component label, should point to any component
                                    ///< config that implements interfaces::PressureSensor
  component_label_t pumpPulseController; ///< Dependency component label, should point to
                                         ///< PulseControlledDeviceConfig
  micrograms_t volumePerPulse;           ///< Volume displaced per pump pulse in micrograms
  millibars_t pumpStallPressure;         ///< Pressure at which pump stalls (with an assumption that
                                         ///< volume per pulse gradually goes to zero when pressure
                                         ///< approaches this value)
};

/**
 * @struct VirtualWeightSensorConfig
 * @brief Configuration for virtual weight sensor derived from flow rate.
 *
 * This virtual sensor calculates weight and flow rate knowing a mass
 * that pump pushes with one pulse and assumes that this mass linearly
 * decreases when pressure grows. This information is sent with callbacks
 * from VibroPumpFlowSensor where actual calculations happen.
 *
 * @note Call interfaces::LibopenpressoCore::getWeightSensor()
 *       with the label of this config to get configured component instance.
 *
 * @see VibroPumpFlowSensor
 */
struct VirtualWeightSensorConfig {
  component_label_t pumpFlowSensor;   ///< Dependency component label, should point to
                                      ///< VibroPumpFlowSensor
  time_delta_t flowRateSmoothingTime; ///< Time constant for flow rate smoothing filter
};

/**
 * @struct Nau7802WeightSensorConfig
 * @brief Configuration for NAU7802 I2C load cell amplifier.
 *
 * 24-bit ADC for load cell weight measurement with configurable scale and smoothing.
 *
 * @note Call interfaces::LibopenpressoCore::getWeightSensor()
 *       with the label of this config to get configured component instance.
 */
struct Nau7802WeightSensorConfig {
  i2c_addr_t addr;      ///< I2C address of the NAU7802 device
  pin_addr_t signalPin; ///< GPIO pin for data ready signal
  uint32_t scale;       ///< ADC count to weight conversion scale (256 results in 1:1 ratio)
  time_delta_t flowRateSmoothingTime; ///< Time constant for flow rate smoothing filter
};

/**
 * @struct VibroPumpFlowController
 * @brief Configuration for vibro pump flow controller.
 *
 * Dynamically recalculates pulses density using the formula
 * from VibroPumpFlowSensor and provides callback for
 * underlying pulse controller.
 *
 * @note Call interfaces::LibopenpressoCore::getFlowRateController()
 *       with the label of this config to get configured component instance.
 *
 * @see VibroPumpFlowSensor
 */
struct VibroPumpFlowController {
  component_label_t pumpFlowSensor; ///< Dependency component label, should point to
                                    ///< VibroPumpFlowSensor
  size_t mainsFrequency;            ///< 50 or 60 Hz depending on country
};

/**
 * @struct IntegralFlowRateControllerConfig
 * @brief Configuration for integral flow rate controller.
 *
 * Calculates output power that should be passed to
 * underlying power controller by integrating differences
 * between target flow rate and sensor readings over time.
 *
 * @note Call interfaces::LibopenpressoCore::getFlowRateController()
 *      with the label of this config to get configured component instance.
 *
 * @see PulsePowerControllerConfig, Nau7802WeightSensorConfig, VirtualWeightSensorConfig
 */
struct IntegralFlowRateControllerConfig {
  component_label_t powerController; ///< Dependency component label, should point to
                                     ///< PulsePowerControllerConfig
  component_label_t sensor;  ///< Dependency component label, should point to any component config
                             ///< that implements interfaces::WeightSensor
  pid_coeffs_t feedbackCoef; ///< Integral feedback coefficient
};

/**
 * @struct PulsePressureControllerConfig
 * @brief Configuration for pulse-based pressure controller.
 *
 * Serves as simple comparator of target pressure and actual pressure
 * that is used as a callback for underlying pulse controller.
 *
 * @note Call interfaces::LibopenpressoCore::getPressureController()
 *       with the label of this config to get configured component instance.
 *
 * @see PulseControlledDeviceConfig, Ads1115PressureSensorConfig
 */
struct PulsePressureControllerConfig {
  component_label_t pulseController; ///< Dependency component label, should point to
                                     ///< PulseControlledDeviceConfig
  component_label_t sensor; ///< Dependency component label, should point to any component config
                            ///< that implements interfaces::PressureSensor
};

/**
 * @struct Max6675TemperatureSensorConfig
 * @brief Configuration for MAX6675 SPI thermocouple amplifier.
 *
 * MAX6675 reads K-type thermocouples via SPI interface.
 *
 * @note Call interfaces::LibopenpressoCore::getTemperatureSensor()
 *       with the label of this config to get configured component instance.
 */
struct Max6675TemperatureSensorConfig {
  unix_dev_addr_t spiDev;               ///< Path to SPI device (e.g., /dev/spidev0.0)
  millidegrees_t watchdogMinValidValue; ///< If sensor redaing is lower than this value, watchdog
                                        ///< reset will be triggered
  millidegrees_t watchdogMaxValidValue; ///< If sensor redaing is higher than this value, watchdog
                                        ///< reset will be triggered
};

/**
 * @struct Max31856TemperatureSensorConfig
 * @brief Configuration for MAX31856 SPI thermocouple amplifier.
 *
 * MAX31856 reads various thermocouple types via SPI with optional GPIO signal line.
 *
 * @note Call interfaces::LibopenpressoCore::getTemperatureSensor()
 *       with the label of this config to get configured component instance.
 */
struct Max31856TemperatureSensorConfig {
  unix_dev_addr_t spiDev;               ///< Path to SPI device (e.g., /dev/spidev0.0)
  pin_addr_t signalPin;                 ///< GPIO pin for data ready signal
  millidegrees_t watchdogMinValidValue; ///< If sensor redaing is lower than this value, watchdog
                                        ///< reset will be triggered
  millidegrees_t watchdogMaxValidValue; ///< If sensor redaing is higher than this value, watchdog
                                        ///< reset will be triggered
};

/**
 * @struct TemperaturePidControllerConfig
 * @brief Configuration for PID-based temperature controller.
 *
 * Maintains target temperature using PID+F feedback loop
 * with optional PID-controller internal state exposure.
 * Feedforward term serves to prevent temperature drops
 * caused by cold water input from the pump.
 *
 * @note Call interfaces::LibopenpressoCore::getTemperatureController()
 *       with the label of this config to get configured component instance.
 *
 * @see PulsePowerControllerConfig, Max6675TemperatureSensorConfig,
 * Max31856TemperatureSensorConfig, VibroPumpFlowSensor, PidSettings
 */
struct TemperaturePidControllerConfig {
  component_label_t powerController; ///< Dependency component label, should point to
                                     ///< PulsePowerControllerConfig
  component_label_t sensor; ///< Dependency component label, should point to any component config
                            ///< that implements interfaces::TemperatureSensor
  PidSettings pidSettings;  ///< PID tuning parameters
  bool enablePidStateDump;  ///< If set, exposes state via interfaces::PidControllerState (use
                            ///< dynamic_cast on the created instance of
                            ///< interfaces::TemperatureController)
  std::optional<component_label_t> flowCounter; ///< Optional dependency component label, should
                                                ///< point to VibroPumpFlowSensor
};

/**
 * @struct LogicalInputPinConfig
 * @brief Configuration for logical input pin with debouncing.
 *
 * GPIO input pin for buttons, switches, and other digital sensors with configurable debounce.
 *
 * @note Call interfaces::LibopenpressoCore::getLogicalInput()
 *       with the label of this config to get configured component instance.
 */
struct LogicalInputPinConfig {
  pin_addr_t addr;             ///< GPIO pin address
  time_delta_t debouncePeriod; ///< Debounce delay to filter noise
  PinPull pull;  ///< Involves internal pull-up/pull-down resistor, if supported by hardware
  bool inverted; ///< Invert logical state (useful for active-low signals)
};

/**
 * @struct LogicalOutputPinConfig
 * @brief Configuration for logical output pin.
 *
 * GPIO output pin for relays, solenoids, LEDs, and other digital actuators.
 */
struct LogicalOutputPinConfig {
  pin_addr_t addr; ///< GPIO pin address
  bool initState;  ///< Initial pin state on initialization
  bool inverted;   ///< Invert logical state (useful for active-low devices)
};

/**
 * @struct BrewProfilerConfig
 * @brief Configuration for automated brew profile executor.
 *
 * Performs step by step brewing process according to predefined profile
 * with the following sequence:
 * 1. Tare weight sensor
 * 2. Open group head valve
 * 3. Execute brew steps from predefined profile
 * 4. Close group head valve
 *
 * @note Call interfaces::LibopenpressoCore::getBrewProfiler()
 *       with the label of this config to get configured component instance.
 *
 * @see Ads1115PressureSensorConfig, IntegralFlowRateControllerConfig, VibroPumpFlowController,
 * LogicalOutputPinConfig, Nau7802WeightSensorConfig, VirtualWeightSensorConfig,
 * interfaces::BrewProfiler
 */
struct BrewProfilerConfig {
  time_delta_t updatePeriod; ///< How often to check conditions for brewing stop or advancing to
                             ///< the next step
  component_label_t pressureController; ///< Dependency component label, should point to any
                                        ///< component config that implements
                                        ///< interfaces::PressureController
  component_label_t flowController;  ///< Dependency component label, should point to any component
                                     ///< config that implements interfaces::FlowRateController
  component_label_t valveController; ///< Dependency component label, should point to any
                                     ///< component config that implements
                                     ///< interfaces::LogicalOutput
  component_label_t weightSensor;    ///< Dependency component label, should point to any component
                                     ///< config that implements interfaces::WeightSensor
};

/**
 * @struct SteamControllerConfig
 * @brief Configuration for steam mode controller.
 *
 * Manages steam boiler heating and refilling with fresh water
 * to compensate water loss during steaming.
 *
 * @note Call interfaces::LibopenpressoCore::getSteamController()
 *       with the label of this config to get configured component instance.
 */
struct SteamControllerConfig {
  millidegrees_t steamTemperature; ///< Target steam boiler temperature
  millibars_t pressureThreshold;   ///< Steam pressure threshold, refill is disabled above this
                                   ///< pressure (should be slightly above maximum possible pressure
                                   ///< with opened steam valve, usually ~2.5 bar)
  millidegrees_t temperatureThreshold; ///< Temperature threshold, refill is disabled below this
                                       ///< temperature (should be ~10°C lower than
                                       ///< steamTemperature)
  milligrams_p_second_t refillFlow;    ///< Boiler refill flow rate (too high values lead to boiler
                                       ///< overflow and water spillage from steam wand, too low
                                       ///< values lead to boiler dry-run during long steaming
                                       ///< sessions)
  time_delta_t refillUpdatePeriod; ///< How often to check if refill is needed (usually 250-500ms
                                   ///< is sufficient)
  component_label_t preheatController; ///< Dependency component label, should point to any
                                       ///< component config that implements
                                       ///< interfaces::TemperatureController
  component_label_t steamingTemperatureController; ///< Dependency component label, should point
                                                   ///< to any component config that implements
                                                   ///< interfaces::TemperatureController
  component_label_t temperatureSensor; ///< Dependency component label, should point to any
                                       ///< component config that implements
                                       ///< interfaces::TemperatureSensor
  component_label_t pressureSensor; ///< Dependency component label, should point to any component
                                    ///< config that implements interfaces::PressureSensor
  component_label_t flowRateController; ///< Dependency component label, should point to
                                        ///< VibroPumpFlowController only!!!
};

/**
 * @struct WatchdogConfig
 * @brief Configuration for system watchdog timer.
 *
 * Hardware or software watchdog for resetting the system
 * in exceptional situations.
 * @remark Usually watchdog has a limitation
 * on minimum timeout that can be set. After an attempt to set desired
 * timeout, actual timeout value is checked against requested timeout,
 * and if it's larger than maxTimeout, exception will be thrown.
 *
 * @note This component isn't exposed directly, but used by the Core during initialization.
 *
 * @see DeviceConfig::watchdog
 */
struct WatchdogConfig {
  unix_dev_addr_t watchdogDev; ///< Path to watchdog device (e.g., /dev/watchdog0)
  time_delta_t timeout;        ///< Desired timeout before automatic reset
};

} // namespace libopenpresso
// NOLINTEND(cppcoreguidelines-pro-type-member-init)

#endif // LIBOPENPRESSO_COMPONENT_CONFIG_HPP