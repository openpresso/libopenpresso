#ifndef CORE_COMPONENTS_FABRIC_POWER_CONTROLLERS_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_POWER_CONTROLLERS_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <memory>

#include <core/core_private_base.hpp>

namespace libopenpresso
{

struct PulsePowerControllerConfig;

class PowerControllerFabric : public ComponentFabricBase<PowerControllerFabric> {
public:
  std::shared_ptr<gpio::PowerController> makeComponent(const PulsePowerControllerConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_POWER_CONTROLLERS_FABRIC_HPP