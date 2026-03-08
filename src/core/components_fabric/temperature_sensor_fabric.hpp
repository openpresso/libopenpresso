#ifndef CORE_COMPONENTS_FABRIC_TEMPERATURE_SENSOR_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_TEMPERATURE_SENSOR_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>

namespace libopenpresso
{

class TemperatureSensorFabric : public ComponentFabricBase<TemperatureSensorFabric> {
public:
  TemperatureSensorPtr makeComponent(const Max31856TemperatureSensorConfig& config);
  TemperatureSensorPtr makeComponent(const Max6675TemperatureSensorConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_TEMPERATURE_SENSOR_FABRIC_HPP