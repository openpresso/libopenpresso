#ifndef CORE_COMPONENTS_FABRIC_PRESSURE_CONTROLLER_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_PRESSURE_CONTROLLER_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/pressure_controller.hpp>

namespace libopenpresso
{

class PressureControllerFabric : public ComponentFabricBase<PressureControllerFabric> {
public:
  PressureControllerPtr makeComponent(const PulsePressureControllerConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_PRESSURE_CONTROLLER_FABRIC_HPP