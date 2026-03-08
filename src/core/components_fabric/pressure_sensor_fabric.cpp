#include "pressure_sensor_fabric.hpp"

#include <memory>
#include <ratio>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <ads1115_pressure_sensor/ads1115_pressure_sensor.hpp>
#include <gpio/pins_manager.hpp>
#include <watchdog/watchdog_manager.hpp>

using namespace libopenpresso;

PressureSensorPtr libopenpresso::PressureSensorFabric::makeComponent(const Ads1115PressureSensorConfig& config)
{
  using namespace std::chrono_literals;

  auto pinMonitor = getPinsManager().findPinMonitor(config.signalPin);
  auto i2cBus = getI2cBus(config.addr.bus);
  auto sensor = std::make_shared<Ads1115PressureSensor>(i2cBus, config.addr.dev, pinMonitor);
  auto fixedUpdateRate = sensor->fixedUpdateRate();
  if (fixedUpdateRate.has_value()) {
    sensor =
      getWatchdog().wrapSensorWithValidator(sensor,
                                            Ads1115PressureSensor::TRANSDUCER_MIN_BARS * std::milli::den,
                                            Ads1115PressureSensor::TRANSDUCER_MAX_BARS * std::milli::den,
                                            fixedUpdateRate.value());
  }
  return sensor;
}
