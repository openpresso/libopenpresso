#ifndef CORE_COMPONENTS_FABRIC_STEAM_CONTROLLER_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_STEAM_CONTROLLER_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/controller_base.hpp>

namespace libopenpresso
{

class SteamControllerFabric : public ComponentFabricBase<SteamControllerFabric> {
public:
  ControllerBasePtr makeComponent(const SteamControllerConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_STEAM_CONTROLLER_FABRIC_HPP