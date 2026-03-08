#ifndef HARDWARE_GPIO_POWER_CONTROLLER_HPP
#define HARDWARE_GPIO_POWER_CONTROLLER_HPP

#include <libopenpresso/types.hpp>

namespace libopenpresso::gpio
{

class PowerController {
public:
  virtual power_units_t powerMax() const noexcept = 0;
  virtual bool isActive() const noexcept = 0;
  virtual void activate() = 0;
  virtual void deactivate() = 0;
  virtual void setTargetPower(power_units_t power) noexcept = 0;
  virtual ~PowerController() = default;
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_POWER_CONTROLLER_HPP