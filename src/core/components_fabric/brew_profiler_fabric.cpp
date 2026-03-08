#include "brew_profiler_fabric.hpp"

#include <memory>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/interfaces/brew_profiler.hpp>

#include <brew_profiler/brew_profiler_impl.hpp>

using namespace libopenpresso;

BrewProfilerPtr BrewProfilerFabric::makeComponent(const BrewProfilerConfig& config)
{
  auto&& pressureController = getPressureController(config.pressureController);
  auto&& flowController = getFlowRateController(config.flowController);
  auto&& valve = getLogicalOutput(config.valveController);
  auto&& weightSensor = getWeightSensor(config.weightSensor);
  return std::make_shared<BrewProfilerImpl>(
    config.updatePeriod, pressureController, flowController, weightSensor, valve);
}
