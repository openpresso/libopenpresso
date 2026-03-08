#ifndef CORE_COMPONENTS_FABRIC_PRESSURE_SENSOR_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_PRESSURE_SENSOR_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>

#include <i2c/i2c_bus_fabric.hpp>

namespace libopenpresso
{

class PressureSensorFabric
: public ComponentFabricBase<PressureSensorFabric>
, public virtual i2c::I2cBusFabric {
public:
  PressureSensorPtr makeComponent(const Ads1115PressureSensorConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_PRESSURE_SENSOR_FABRIC_HPP