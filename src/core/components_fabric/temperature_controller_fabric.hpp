#ifndef CORE_COMPONENTS_FABRIC_TEMPERATURE_CONTROLLER_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_TEMPERATURE_CONTROLLER_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/temperature_controller.hpp>

namespace libopenpresso
{

class TemperatureControllerFabric : public ComponentFabricBase<TemperatureControllerFabric> {
public:
  TemperatureControllerPtr makeComponent(const TemperaturePidControllerConfig& config);
  TemperatureControllerPtr makeComponent(const SteamControllerConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_TEMPERATURE_CONTROLLER_FABRIC_HPP