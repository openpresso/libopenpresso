#ifndef CORE_COMPONENTS_FABRIC_BREW_PROFILER_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_BREW_PROFILER_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/brew_profiler.hpp>

namespace libopenpresso
{

class BrewProfilerFabric : public ComponentFabricBase<BrewProfilerFabric> {
public:
  BrewProfilerPtr makeComponent(const BrewProfilerConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_BREW_PROFILER_FABRIC_HPP