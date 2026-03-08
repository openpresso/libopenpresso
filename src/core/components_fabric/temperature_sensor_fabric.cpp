#include "temperature_sensor_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pins_manager.hpp>
#include <max31856_temperature_sensor/max31856_temperature_sensor.hpp>
#include <max6675_temperature_sensor/max6675_temperature_sensor.hpp>
#include <watchdog/watchdog_manager.hpp>

using namespace libopenpresso;

TemperatureSensorPtr TemperatureSensorFabric::makeComponent(const Max31856TemperatureSensorConfig& config)
{
  using namespace std::chrono_literals;

  auto pinMonitor = getPinsManager().findPinMonitor(config.signalPin);
  auto sensor = std::make_shared<Max31856TemperatureSensor>(config.spiDev, pinMonitor);
  auto fixedUpdateRate = sensor->fixedUpdateRate();
  if (fixedUpdateRate.has_value()) {
    sensor = getWatchdog().wrapSensorWithValidator(
      sensor, config.watchdogMinValidValue, config.watchdogMaxValidValue, fixedUpdateRate.value());
  }
  return sensor;
}

TemperatureSensorPtr TemperatureSensorFabric::makeComponent(const Max6675TemperatureSensorConfig& config)
{
  using namespace std::chrono_literals;

  auto sensor = std::make_shared<Max6675TemperatureSensor>(config.spiDev);
  auto fixedUpdateRate = sensor->fixedUpdateRate();
  if (fixedUpdateRate.has_value()) {
    sensor = getWatchdog().wrapSensorWithValidator(
      sensor, config.watchdogMinValidValue, config.watchdogMaxValidValue, fixedUpdateRate.value());
  }
  return sensor;
}