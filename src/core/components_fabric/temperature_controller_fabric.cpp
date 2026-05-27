#include "temperature_controller_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/temperature_controller.hpp>

#include <pid_temperature_controller/pid_temperature_controller.hpp>
#include <steam_controller/steam_controller.hpp>

using namespace libopenpresso;

TemperatureControllerPtr TemperatureControllerFabric::makeComponent(const TemperaturePidControllerConfig& config)
{
  auto powerController = getPowerController(config.powerController);
  auto temperatureSensor = getTemperatureSensor(config.sensor);
  std::shared_ptr<PumpFlowSensor> flowRate;
  if (config.flowCounter.has_value()) {
    flowRate = getFlowCounter(config.flowCounter.value());
  }

  if (config.enablePidStateDump) {
    return std::make_shared<PidTemperatureController<true>>(
      powerController, temperatureSensor, flowRate, config.pidSettings);
  }
  return std::make_shared<PidTemperatureController<false>>(
    powerController, temperatureSensor, flowRate, config.pidSettings);
}

TemperatureControllerPtr libopenpresso::TemperatureControllerFabric::makeComponent(
  const SteamControllerConfig& config)
{
  auto&& flowController = getFlowRateController(config.flowRateController);
  auto&& preheatController = getTemperatureController(config.preheatController);
  auto&& steamingTemperatureController = getTemperatureController(config.steamingTemperatureController);
  auto&& temperatureSensor = getTemperatureSensor(config.temperatureSensor);
  auto&& pressureSensor = getPressureSensor(config.pressureSensor);
  return std::make_shared<SteamController>(temperatureSensor,
                                           preheatController,
                                           steamingTemperatureController,
                                           config.temperatureRelativeThreshold,
                                           pressureSensor,
                                           config.pressureThreshold,
                                           flowController,
                                           config.refillFlow,
                                           config.refillUpdatePeriod);
}
