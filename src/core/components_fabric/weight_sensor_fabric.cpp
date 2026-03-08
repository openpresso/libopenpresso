#include "weight_sensor_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>

#include <gpio/pins_manager.hpp>
#include <nau7802_weight_sensor/nau7802_weight_sensor.hpp>
#include <virtual_weight_sensor/virtual_weight_sensor.hpp>

using namespace libopenpresso;

WeightSensorPtr WeightSensorFabric::makeComponent(const VirtualWeightSensorConfig& config)
{
  return std::make_shared<VirtualWeightSensor>(getFlowCounter(config.pumpFlowSensor),
                                               config.flowRateSmoothingTime);
}

WeightSensorPtr libopenpresso::WeightSensorFabric::makeComponent(const Nau7802WeightSensorConfig& config)
{
  auto bus = getI2cBus(config.addr.bus);
  auto pinMonitor = getPinsManager().findPinMonitor(config.signalPin);
  return std::make_shared<Nau7802WeightSensor>(
    bus, config.addr.dev, pinMonitor, config.scale, config.flowRateSmoothingTime);
}
