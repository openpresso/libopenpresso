#include "steam_controller_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/controller_base.hpp>

#include <steam_controller/steam_controller.hpp>

using namespace libopenpresso;

ControllerBasePtr SteamControllerFabric::makeComponent(const SteamControllerConfig& config)
{
  auto&& flowController = getFlowRateController(config.flowRateController);
  auto&& preheatController = getTemperatureController(config.preheatController);
  auto&& steamingTemperatureController = getTemperatureController(config.steamingTemperatureController);
  auto&& temperatureSensor = getTemperatureSensor(config.temperatureSensor);
  auto&& pressureSensor = getPressureSensor(config.pressureSensor);
  return std::make_shared<SteamController>(temperatureSensor,
                                           preheatController,
                                           steamingTemperatureController,
                                           config.steamTemperature,
                                           config.temperatureThreshold,
                                           pressureSensor,
                                           config.pressureThreshold,
                                           flowController,
                                           config.refillFlow,
                                           config.refillUpdatePeriod);
}
