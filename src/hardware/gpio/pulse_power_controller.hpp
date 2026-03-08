#ifndef HARDWARE_GPIO_PULSE_POWER_CONTROLLER_HPP
#define HARDWARE_GPIO_PULSE_POWER_CONTROLLER_HPP

#include "power_controller.hpp"

#include <atomic>
#include <cstdint>
#include <memory>

#include <libopenpresso/types.hpp>

namespace libopenpresso::gpio
{

class PulseController;

class PulsePowerController : public PowerController {
public:
  PulsePowerController(const std::shared_ptr<PulseController>& pulseController, uint8_t maxPower);

  power_units_t powerMax() const noexcept override;
  bool isActive() const noexcept override;
  void activate() override;
  void deactivate() override;
  void setTargetPower(power_units_t power) noexcept override;

private:
  bool zeroCrossCallback();

private:
  std::atomic<uint8_t> m_target;
  uint8_t m_errorAccum = 0;
  uint8_t m_threshold = 0;
  bool m_isActive = false;
  std::shared_ptr<PulseController> m_pulseController;
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_PULSE_POWER_CONTROLLER_HPP