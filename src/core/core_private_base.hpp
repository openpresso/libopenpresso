#ifndef CORE_CORE_PRIVATE_BASE_HPP
#define CORE_CORE_PRIVATE_BASE_HPP

#include <memory>

#include <libopenpresso/config.hpp>
#include <libopenpresso/types.hpp>

namespace libopenpresso
{

class PumpFlowSensor;
namespace watchdog
{
class WatchdogManager;
}
namespace gpio
{
class PinsManager;
class PowerController;
class PulseController;
} // namespace gpio

class CorePrivateBase {
public:
  virtual const component_config_t& findComponentConfig(const component_label_t& label) const = 0;
  virtual watchdog::WatchdogManager& getWatchdog() = 0;
  virtual const gpio::PinsManager& getPinsManager() const = 0;
  virtual std::shared_ptr<PumpFlowSensor> getFlowCounter(const component_label_t& label) = 0;
  virtual std::shared_ptr<gpio::PulseController> getPulseController(const component_label_t& label) = 0;
  virtual std::shared_ptr<gpio::PowerController> getPowerController(const component_label_t& label) = 0;
  virtual ~CorePrivateBase() = default;
};

} // namespace libopenpresso

#endif // CORE_CORE_PRIVATE_BASE_HPP