#include "flow_sensor_fabric.hpp"

#include <memory>
#include <ranges>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/config.hpp>
#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <pump_flow_sensor/pump_flow_sensor.hpp>

using namespace libopenpresso;

void FlowSensorFabric::init(const components_map_t& components)
{
  m_flowCounters = makeFlowSensorsMap(components);
}

std::shared_ptr<PumpFlowSensor> FlowSensorFabric::getComponent(const component_label_t& label)
{
  auto it = m_flowCounters.find(label);
  if (it == m_flowCounters.end()) {
    throw libopenpresso::Exception{"Flow counter with label {} doesn't exist", label};
  }

  return it->second;
}

std::shared_ptr<PumpFlowSensor> FlowSensorFabric::makeFlowSensor(const VibroPumpFlowSensor& config)
{
  auto pulseController = getPulseController(config.pumpPulseController);
  auto pressureSensor = getPressureSensor(config.pressureSensor);
  auto counter =
    std::make_shared<PumpFlowSensor>(pressureSensor, config.pumpStallPressure, config.volumePerPulse);
  pulseController->setPulseCounter(counter);
  return counter;
}

FlowSensorFabric::counters_map_t FlowSensorFabric::makeFlowSensorsMap(const components_map_t& components)
{
  return std::ranges::to<counters_map_t>(
    components | std::views::filter([](auto&& comp) {
      return std::holds_alternative<VibroPumpFlowSensor>(comp.second);
    }) |
    std::views::transform([this](auto&& comp) {
      return std::make_pair(comp.first, makeFlowSensor(std::get<VibroPumpFlowSensor>(comp.second)));
    }));
}
