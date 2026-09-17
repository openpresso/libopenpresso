#ifndef CONTROLLERS_INTEGRAL_FLOW_RATE_CONTROLLER_INTEGRAL_FLOW_RATE_CONTROLLER_HPP
#define CONTROLLERS_INTEGRAL_FLOW_RATE_CONTROLLER_INTEGRAL_FLOW_RATE_CONTROLLER_HPP

#include <atomic>
#include <chrono>
#include <memory>
#include <optional>

#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <utils/callback_descriptor_handler.hpp>

namespace libopenpresso
{

namespace interfaces
{
class WeightSensor;
}

namespace gpio
{
class PowerController;
}

class IntegralFlowRateController final : public interfaces::FlowRateController {
public:
  IntegralFlowRateController(std::shared_ptr<gpio::PowerController> powerController,
                             WeightSensorPtr sensor,
                             pid_coeffs_t coef);
  IntegralFlowRateController(const IntegralFlowRateController&) = delete;
  IntegralFlowRateController(IntegralFlowRateController&&) = delete;
  auto operator=(const IntegralFlowRateController&) -> IntegralFlowRateController& = delete;
  auto operator=(IntegralFlowRateController&&) -> IntegralFlowRateController& = delete;
  ~IntegralFlowRateController() = default;

  void activate() override;
  void deactivate() override;
  bool isActive() const noexcept override;
  milligrams_p_second_t getTargetRate() const override;
  void setTargetRate(milligrams_p_second_t rate) override;

private:
  void weightSensorCallback(
    milligrams_t weight, milligrams_p_second_t rate, std::chrono::duration<float> dt, float& accum);
  auto getFixedUpdateRateCallback(time_delta_t updateRate);
  auto getVariableUpdateRateCallback();

private:
  const pid_coeffs_t m_coef = 0.0f;
  std::chrono::steady_clock::time_point m_prevTime = std::chrono::steady_clock::now();
  std::atomic<milligrams_p_second_t> m_targetRate = 0;
  std::shared_ptr<gpio::PowerController> m_control;
  WeightSensorPtr m_sensor;
  std::optional<CallbackDescriptorHandler> m_cbDescriptor;
};

} // namespace libopenpresso

#endif // CONTROLLERS_INTEGRAL_FLOW_RATE_CONTROLLER_INTEGRAL_FLOW_RATE_CONTROLLER_HPP