#include "flow_rate_controller_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>

#include <integral_flow_rate_controller/integral_flow_rate_controller.hpp>
#include <virtual_flow_rate_controller/virtual_flow_rate_controller.hpp>

using namespace libopenpresso;

FlowRateControllerPtr FlowRateControllerFabric::makeComponent(const IntegralFlowRateControllerConfig& config)
{
  auto powerController = getPowerController(config.powerController);
  auto weightSensor = getWeightSensor(config.sensor);

  return std::make_shared<IntegralFlowRateController>(powerController, weightSensor, config.feedbackCoef);
}

FlowRateControllerPtr FlowRateControllerFabric::makeComponent(const VibroPumpFlowController& config)
{
  auto&& flowSensorConfig = std::get<VibroPumpFlowSensor>(findComponentConfig(config.pumpFlowSensor));
  auto pressureSensor = getPressureSensor(flowSensorConfig.pressureSensor);
  auto pulseController = getPulseController(flowSensorConfig.pumpPulseController);
  return std::make_shared<VirtualFlowRateController>(pulseController,
                                                     pressureSensor,
                                                     flowSensorConfig.pumpStallPressure,
                                                     flowSensorConfig.volumePerPulse,
                                                     config.mainsFrequency);
}
