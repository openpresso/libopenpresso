#ifndef CORE_COMPONENTS_FABRIC_PULSE_CONTROLLER_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_PULSE_CONTROLLER_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <memory>

#include <core/core_private_base.hpp>

namespace libopenpresso
{

struct PulseControlledDeviceConfig;

class PulseControllerFabric : public ComponentFabricBase<PulseControllerFabric> {
public:
  std::shared_ptr<gpio::PulseController> makeComponent(const PulseControlledDeviceConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_PULSE_CONTROLLER_FABRIC_HPP