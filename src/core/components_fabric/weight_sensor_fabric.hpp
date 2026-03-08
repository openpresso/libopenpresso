#ifndef CORE_COMPONENTS_FABRIC_WEIGHT_SENSOR_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_WEIGHT_SENSOR_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>

#include <i2c/i2c_bus_fabric.hpp>

namespace libopenpresso
{

class WeightSensorFabric
: public ComponentFabricBase<WeightSensorFabric>
, public virtual i2c::I2cBusFabric {
public:
  WeightSensorPtr makeComponent(const VirtualWeightSensorConfig& config);
  WeightSensorPtr makeComponent(const Nau7802WeightSensorConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_WEIGHT_SENSOR_FABRIC_HPP