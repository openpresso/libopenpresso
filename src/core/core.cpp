#include "core.hpp"

#include "components_pins.hpp"

#include <list>
#include <memory>
#include <ranges>

#include <libopenpresso/config.hpp>
#include <libopenpresso/exception.hpp>
#include <libopenpresso/interfaces/brew_profiler.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/logical_input.hpp>
#include <libopenpresso/interfaces/logical_output.hpp>
#include <libopenpresso/interfaces/pressure_controller.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/interfaces/temperature_controller.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <gpio/power_controller.hpp>
#include <gpio/pulse_controller.hpp>
#include <pump_flow_sensor/pump_flow_sensor.hpp>
#include <utils/logger.hpp>

using namespace libopenpresso;

Core::Core(const DeviceConfig& config)
: m_configs{config.components}
, m_watchdog{config.watchdog}
, m_pins{std::ranges::to<std::list<gpio::any_pin_info_t>>(config.components |
                                                          components_pins::components_pin_view)}
{
  Logger::debug("Libopenpresso core created");
}

Core::~Core()
{
  Logger::debug("Libopenpresso core destroyed");
}

void Core::init()
{
  FlowSensorFabric::init(m_configs);
  Logger::debug("Libopenpresso core inited");
}

WeightSensorPtr Core::getWeightSensor(const component_label_t& label)
{
  return WeightSensorFabric::getComponent(label);
}

FlowRateControllerPtr Core::getFlowRateController(const component_label_t& label)
{
  return FlowRateControllerFabric::getComponent(label);
}

PressureSensorPtr Core::getPressureSensor(const component_label_t& label)
{
  return PressureSensorFabric::getComponent(label);
}

PressureControllerPtr Core::getPressureController(const component_label_t& label)
{
  return PressureControllerFabric::getComponent(label);
}

TemperatureSensorPtr Core::getTemperatureSensor(const component_label_t& label)
{
  return TemperatureSensorFabric::getComponent(label);
}

TemperatureControllerPtr Core::getTemperatureController(const component_label_t& label)
{
  return TemperatureControllerFabric::getComponent(label);
}

LogicalOutputPtr Core::getLogicalOutput(const component_label_t& label)
{
  return LogicalOutputFabric::getComponent(label);
}

LogicalInputPtr Core::getLogicalInput(const component_label_t& label)
{
  return LogicalInputFabric::getComponent(label);
}

BrewProfilerPtr Core::getBrewProfiler(const component_label_t& label)
{
  return BrewProfilerFabric::getComponent(label);
}

const component_config_t& Core::findComponentConfig(const component_label_t& label) const
{
  auto componentIt = m_configs.find(label);
  if (componentIt == m_configs.end()) {
    throw libopenpresso::Exception{"Component label {} not found", label};
  }
  return componentIt->second;
}

watchdog::WatchdogManager& Core::getWatchdog()
{
  return m_watchdog;
}

const gpio::PinsManager& Core::getPinsManager() const
{
  return m_pins;
}

std::shared_ptr<PumpFlowSensor> Core::getFlowCounter(const component_label_t& label)
{
  return FlowSensorFabric::getComponent(label);
}

std::shared_ptr<gpio::PulseController> Core::getPulseController(const component_label_t& label)
{
  return PulseControllerFabric::getComponent(label);
}

std::shared_ptr<gpio::PowerController> Core::getPowerController(const component_label_t& label)
{
  return PowerControllerFabric::getComponent(label);
}
