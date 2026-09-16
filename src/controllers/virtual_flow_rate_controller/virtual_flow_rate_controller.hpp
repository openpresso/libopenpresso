#ifndef CONTROLLERS_VIRTUAL_FLOW_RATE_CONTROLLER_VIRTUAL_FLOW_RATE_CONTROLLER_HPP
#define CONTROLLERS_VIRTUAL_FLOW_RATE_CONTROLLER_VIRTUAL_FLOW_RATE_CONTROLLER_HPP

#include <atomic>
#include <cstddef>
#include <memory>

#include <libopenpresso/interfaces/flow_rate_controller.hpp>
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
class VirtualFlowRateController final : public interfaces::FlowRateController {
public:
  VirtualFlowRateController(std::shared_ptr<gpio::PulseController> controller,
                            PressureSensorPtr sensor,
                            millibars_t pumpStallPressure,
                            micrograms_t volumePerPulse,
                            size_t mainsFrequency);
  VirtualFlowRateController(const VirtualFlowRateController&) = delete;
  VirtualFlowRateController(VirtualFlowRateController&&) = delete;
  auto operator=(const VirtualFlowRateController&) = delete;
  auto operator=(VirtualFlowRateController&&) = delete;
  ~VirtualFlowRateController() = default;

  void activate() override;
  void deactivate() override;
  bool isActive() const noexcept override;
  milligrams_p_second_t getTargetRate() const override;
  void setTargetRate(milligrams_p_second_t rate) override;

private:
  auto getPulseCallback() const;

private:
  std::shared_ptr<gpio::PulseController> m_controller;
  PressureSensorPtr m_sensor;
  const millibars_t m_pumpStallPressure;
  const micrograms_t m_volumePerPulse;
  const size_t m_mainsFrequency;
  bool m_isActive = false;
  std::atomic<milligrams_p_second_t> m_targetRate = 0;
};

} // namespace libopenpresso

#endif // CONTROLLERS_VIRTUAL_FLOW_RATE_CONTROLLER_VIRTUAL_FLOW_RATE_CONTROLLER_HPP