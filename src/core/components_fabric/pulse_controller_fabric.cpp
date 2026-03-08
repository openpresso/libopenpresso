#include "pulse_controller_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>

#include <gpio/pulse_controller.hpp>
#include <watchdog/watchdog_manager.hpp>

using namespace libopenpresso;

std::shared_ptr<gpio::PulseController> PulseControllerFabric::makeComponent(
  const PulseControlledDeviceConfig& config)
{
  auto&& output = getWatchdog().getPinOutput(config.pulsePin, getPinsManager());
  auto&& acSensorConfig =
    std::get<AcZeroCrossSensorConfig>(findComponentConfig(config.acZeroCrossSensor));
  auto&& monitor = getWatchdog().getAcMonitor(acSensorConfig.signalPin, getPinsManager());
  return std::make_shared<gpio::PulseController>(monitor, output);
}
