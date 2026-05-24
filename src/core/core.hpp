#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include "components_fabric/brew_profiler_fabric.hpp"
#include "components_fabric/flow_rate_controller_fabric.hpp"
#include "components_fabric/flow_sensor_fabric.hpp"
#include "components_fabric/logical_input_fabric.hpp"
#include "components_fabric/logical_output_fabric.hpp"
#include "components_fabric/power_controllers_fabric.hpp"
#include "components_fabric/pressure_controller_fabric.hpp"
#include "components_fabric/pressure_sensor_fabric.hpp"
#include "components_fabric/pulse_controller_fabric.hpp"
#include "components_fabric/temperature_controller_fabric.hpp"
#include "components_fabric/temperature_sensor_fabric.hpp"
#include "components_fabric/weight_sensor_fabric.hpp"
#include "core_private_base.hpp"

#include <memory>

#include <libopenpresso/config.hpp>
#include <libopenpresso/interfaces/brew_profiler.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/libopenpresso_core.hpp>
#include <libopenpresso/interfaces/logical_input.hpp>
#include <libopenpresso/interfaces/logical_output.hpp>
#include <libopenpresso/interfaces/pressure_controller.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/interfaces/temperature_controller.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pins_manager.hpp>
#include <watchdog/watchdog_manager.hpp>

namespace libopenpresso
{

class Core
: public virtual interfaces::LibopenpressoCore
, private virtual CorePrivateBase
, private PulseControllerFabric
, private PowerControllerFabric
, private FlowSensorFabric
, private WeightSensorFabric
, private PressureSensorFabric
, private TemperatureSensorFabric
, private LogicalInputFabric
, private PressureControllerFabric
, private FlowRateControllerFabric
, private TemperatureControllerFabric
, private LogicalOutputFabric
, private BrewProfilerFabric {
public:
  Core(const DeviceConfig& config);
  Core(const Core&) = delete;
  Core(Core&&) = delete;
  auto operator=(const Core&) = delete;
  auto operator=(Core&&) = delete;
  ~Core();

  void init();

  WeightSensorPtr getWeightSensor(const component_label_t& label) override;
  FlowRateControllerPtr getFlowRateController(const component_label_t& label) override;
  PressureSensorPtr getPressureSensor(const component_label_t& label) override;
  PressureControllerPtr getPressureController(const component_label_t& label) override;
  TemperatureSensorPtr getTemperatureSensor(const component_label_t& label) override;
  TemperatureControllerPtr getTemperatureController(const component_label_t& label) override;
  LogicalOutputPtr getLogicalOutput(const component_label_t& label) override;
  LogicalInputPtr getLogicalInput(const component_label_t& label) override;
  BrewProfilerPtr getBrewProfiler(const component_label_t& label) override;

private:
  const component_config_t& findComponentConfig(const component_label_t& label) const override;
  watchdog::WatchdogManager& getWatchdog() override;
  const gpio::PinsManager& getPinsManager() const override;
  std::shared_ptr<PumpFlowSensor> getFlowCounter(const component_label_t& label) override;
  std::shared_ptr<gpio::PulseController> getPulseController(const component_label_t& label) override;
  std::shared_ptr<gpio::PowerController> getPowerController(const component_label_t& label) override;

private:
  components_map_t m_configs;
  watchdog::WatchdogManager m_watchdog;
  gpio::PinsManager m_pins;
};

} // namespace libopenpresso

#endif // CORE_CORE_HPP