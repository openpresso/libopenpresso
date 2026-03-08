#include "pressure_controller_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/pressure_controller.hpp>

#include <pulse_pressure_controller/pulse_pressure_controller.hpp>

using namespace libopenpresso;

PressureControllerPtr libopenpresso::PressureControllerFabric::makeComponent(
  const PulsePressureControllerConfig& config)
{
  auto pulseController = getPulseController(config.pulseController);
  auto pressureSensor = getPressureSensor(config.sensor);
  return std::make_shared<PulsePressureController>(pulseController, pressureSensor);
}
