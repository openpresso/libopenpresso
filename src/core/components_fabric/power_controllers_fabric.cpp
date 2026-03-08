#include "power_controllers_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>

#include <gpio/power_controller.hpp>
#include <gpio/pulse_power_controller.hpp>

using namespace libopenpresso;

std::shared_ptr<gpio::PowerController> PowerControllerFabric::makeComponent(const PulsePowerControllerConfig& config)
{
  auto pulseController = getPulseController(config.pulseControlledDevice);
  return std::make_shared<gpio::PulsePowerController>(pulseController, config.dutyCycle);
}
