#ifndef CORE_COMPONENTS_FABRIC_LOGICAL_OUTPUT_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_LOGICAL_OUTPUT_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/logical_output.hpp>

namespace libopenpresso
{

class LogicalOutputFabric : public ComponentFabricBase<LogicalOutputFabric> {
public:
  LogicalOutputPtr makeComponent(const LogicalOutputPinConfig& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_LOGICAL_OUTPUT_FABRIC_HPP
