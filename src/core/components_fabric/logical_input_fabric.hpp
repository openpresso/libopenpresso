#ifndef CORE_COMPONENTS_FABRIC_LOGICAL_INPUT_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_LOGICAL_INPUT_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/logical_input.hpp>

namespace libopenpresso
{

class LogicalInputFabric : public ComponentFabricBase<LogicalInputFabric> {
public:
  LogicalInputPtr makeComponent(const LogicalInputPinConfig& config) const;
  LogicalInputPtr makeComponent(const AcZeroCrossSensorConfig& config) const;
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_LOGICAL_INPUT_FABRIC_HPP