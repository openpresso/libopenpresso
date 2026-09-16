#ifndef CONTROLLERS_PULSE_PRESSURE_CONTROLLER_PULSE_PRESSURE_CONTROLLER_HPP
#define CONTROLLERS_PULSE_PRESSURE_CONTROLLER_PULSE_PRESSURE_CONTROLLER_HPP

#include <atomic>
#include <memory>

#include <libopenpresso/interfaces/pressure_controller.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/types.hpp>

namespace libopenpresso
{

namespace gpio
{
class PulseController;
}
namespace interfaces
{
class PressureSensor;
}

class PulsePressureController final : public interfaces::PressureController {
public:
  PulsePressureController(std::shared_ptr<gpio::PulseController> controller, PressureSensorPtr sensor);
  PulsePressureController(const PulsePressureController&) = delete;
  PulsePressureController(PulsePressureController&&) = delete;
  auto operator=(const PulsePressureController&) = delete;
  auto operator=(PulsePressureController&&) = delete;
  ~PulsePressureController();

  void activate() override;
  void deactivate() override;
  bool isActive() const noexcept override;
  millibars_t getTargetPressure() const override;
  void setTargetPressure(millibars_t pressure) override;

private:
  std::shared_ptr<gpio::PulseController> m_control;
  PressureSensorPtr m_sensor;
  std::atomic<millibars_t> m_targetPressure = 0;
  bool m_isActive = false;
};

} // namespace libopenpresso

#endif // CONTROLLERS_PULSE_PRESSURE_CONTROLLER_PULSE_PRESSURE_CONTROLLER_HPP