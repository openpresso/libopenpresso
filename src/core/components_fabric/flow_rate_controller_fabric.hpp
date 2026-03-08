#ifndef CORE_COMPONENTS_FABRIC_FLOW_RATE_CONTROLLER_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_FLOW_RATE_CONTROLLER_FABRIC_HPP

#include "component_fabric_base.hpp"

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>

namespace libopenpresso
{

class FlowRateControllerFabric : public ComponentFabricBase<FlowRateControllerFabric> {
public:
  FlowRateControllerPtr makeComponent(const IntegralFlowRateControllerConfig& config);
  FlowRateControllerPtr makeComponent(const VibroPumpFlowController& config);
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_FLOW_RATE_CONTROLLER_FABRIC_HPP