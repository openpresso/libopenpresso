#ifndef CORE_COMPONENTS_FABRIC_FLOW_SENSOR_FABRIC_HPP
#define CORE_COMPONENTS_FABRIC_FLOW_SENSOR_FABRIC_HPP

#include <memory>
#include <unordered_map>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/config.hpp>
#include <libopenpresso/interfaces/libopenpresso_core.hpp>
#include <libopenpresso/types.hpp>

#include <core/core_private_base.hpp>

namespace libopenpresso
{

class FlowSensorFabric
: private virtual CorePrivateBase
, private virtual interfaces::LibopenpressoCore {
  using counters_map_t = std::unordered_map<component_label_t, std::shared_ptr<PumpFlowSensor>>;

public:
  void init(const components_map_t& components);
  std::shared_ptr<PumpFlowSensor> getComponent(const component_label_t& label);

private:
  std::shared_ptr<PumpFlowSensor> makeFlowSensor(const VibroPumpFlowSensor& config);
  counters_map_t makeFlowSensorsMap(const components_map_t& components);

private:
  counters_map_t m_flowCounters;
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_FLOW_SENSOR_FABRIC_HPP