/**
 * @file libopenpresso.hpp
 * @brief Main entry point for libopenpresso initialization.
 *
 * Provides the core initialization function to create an LibopenpressoCore instance
 * from device configuration. This is the primary API entry point for users
 * of the openpresso library.
 */

#ifndef LIBOPENPRESSO_LIBOPENPRESSO_HPP
#define LIBOPENPRESSO_LIBOPENPRESSO_HPP

// NOLINTBEGIN(misc-include-cleaner)
#include <libopenpresso/config.hpp>
#include <libopenpresso/interfaces/brew_profiler.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/libopenpresso_core.hpp>
#include <libopenpresso/interfaces/logical_input.hpp>
#include <libopenpresso/interfaces/logical_output.hpp>
#include <libopenpresso/interfaces/pid_controller_state.hpp>
#include <libopenpresso/interfaces/pressure_controller.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/interfaces/temperature_controller.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>
// NOLINTEND(misc-include-cleaner)

/**
 * @brief Top level libopenpresso namespace where all the entities are defined
 */
namespace libopenpresso
{

/**
 * @fn std::shared_ptr<interfaces::LibopenpressoCore> getCore(const DeviceConfig& config)
 * @brief Creates and initializes the openpresso core instance.
 *
 * This is the primary entry point for libopenpresso. Creates a singleton-like
 * Core instance that manages all hardware components (sensors, controllers,
 * I/O devices) as configured.
 *
 * All components created by the Core remain accessible as long as Core exists
 * because child components shared pointers are held internally by the Core.
 *
 * <h3> Typical usage: </h3>
 * @code
 * auto core = libopenpresso::getCore(myDeviceConfig);
 * auto pressure = core->getPressureSensor("main_pressure");
 * auto flow = core->getFlowRateController("pump");
 * @endcode
 *
 * @note Should typically be called once at application startup and held
 *   for the application lifetime. However, child components can
 *   outlive the Core if held separately.
 *
 * @param[in] config Device configuration describing all components to be created,
 *            their parameters, and their inter-dependencies. See DeviceConfig.
 *
 * @return std::shared_ptr<@ref interfaces::LibopenpressoCore> Shared pointer to the
 *         initialized core instance implementing the LibopenpressoCore interface.
 *         Check for nullptr isn't required as any fail will raise an exception.
 *
 * @throws libopenpresso::Exception If initialization fails (invalid config,
 *         missing hardware, etc.). The exact error message describes what failed.
 *
 * @see DeviceConfig, interfaces::LibopenpressoCore
 */
CorePtr getCore(const DeviceConfig& config);

} // namespace libopenpresso

#endif // LIBOPENPRESSO_LIBOPENPRESSO_HPP