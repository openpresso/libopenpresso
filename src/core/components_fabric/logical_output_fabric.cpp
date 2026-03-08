#include "logical_output_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/logical_output.hpp>

#include <pin_output_logical_switch/pin_output_logical_switch.hpp>
#include <watchdog/watchdog_manager.hpp>

using namespace libopenpresso;

LogicalOutputPtr LogicalOutputFabric::makeComponent(const LogicalOutputPinConfig& config)
{
  return std::make_shared<PinOutputLogicalSwitch>(
    getWatchdog().getPinOutput(config.addr, getPinsManager()), config.inverted);
}
