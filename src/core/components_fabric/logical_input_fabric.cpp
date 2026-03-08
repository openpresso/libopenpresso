#include "logical_input_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/logical_input.hpp>
#include <libopenpresso/types.hpp>

#include <debounced_pin_state/debounced_pin_state.hpp>
#include <gpio/pins_manager.hpp>

using namespace libopenpresso;

LogicalInputPtr LogicalInputFabric::makeComponent(const LogicalInputPinConfig& config) const
{
  auto pinMonitor = getPinsManager().findPinMonitor(config.addr);
  return std::make_shared<SwDebouncedButtonState>(pinMonitor, config.debouncePeriod, config.inverted);
}

LogicalInputPtr LogicalInputFabric::makeComponent(const AcZeroCrossSensorConfig& config) const
{
  auto pinMonitor = getPinsManager().findPinMonitor(config.signalPin);
  return std::make_shared<SwDebouncedButtonState>(pinMonitor, time_delta_t{0}, false);
}