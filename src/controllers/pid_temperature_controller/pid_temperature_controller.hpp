#ifndef CONTROLLERS_PID_TEMPERATURE_CONTROLLER_PID_TEMPERATURE_CONTROLLER_HPP
#define CONTROLLERS_PID_TEMPERATURE_CONTROLLER_PID_TEMPERATURE_CONTROLLER_HPP

#include "pid_state_dump.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <optional>
#include <type_traits>
#include <variant>

#include <libopenpresso/interfaces/temperature_controller.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/pid_settings.hpp>
#include <libopenpresso/types.hpp>

#include <utils/callback_descriptor_handler.hpp>

namespace libopenpresso
{

namespace interfaces
{
class TemperatureSensor;
}
namespace gpio
{
class PowerController;
}

class PumpFlowSensor;

template <bool dumpPidState = true>
class PidTemperatureController final
: public interfaces::TemperatureController
, public std::conditional_t<dumpPidState, PidStateDump, std::monostate> {
  static constexpr pid_calc_t PID_RESULT_MAX = 1.0;

public:
  PidTemperatureController(std::shared_ptr<gpio::PowerController> powerController,
                           TemperatureSensorPtr temperatureSensor,
                           std::shared_ptr<PumpFlowSensor> flowSensor,
                           const PidSettings& pidSettings);

  PidTemperatureController(const PidTemperatureController&) = delete;
  PidTemperatureController(PidTemperatureController&&) = delete;
  auto operator=(const PidTemperatureController&) = delete;
  auto operator=(PidTemperatureController&&) = delete;
  ~PidTemperatureController() = default;

  void activate() override;
  void deactivate() override;
  bool isActive() const noexcept override;
  millidegrees_t getTargetTemperature() const override;
  void setTargetTemperature(millidegrees_t millidegrees) override;

private:
  void flowCallback(micrograms_t weight);
  pid_calc_t getRelaxedDcoef(pid_calc_t error);
  pid_calc_t getRelaxedIcoef(pid_calc_t pidSum);
  static PidSettings applyUntisMutlipliers(PidSettings settings) noexcept;
  auto makeFixedUpdateRateCallback(std::chrono::duration<float> dt);
  auto makeVariableUpdateRateCallback();
  void temperatureSensorCallback(
    millidegrees_t temp, std::chrono::duration<float> dt, auto& dFilter, pid_calc_t& iTerm, pid_calc_t& wTerm);

private:
  std::shared_ptr<gpio::PowerController> m_powerController;

  TemperatureSensorPtr m_temperatureSensor;
  std::shared_ptr<PumpFlowSensor> m_flowSensor;
  std::optional<CallbackDescriptorHandler> m_tempSensorCbDescr;
  std::optional<CallbackDescriptorHandler> m_flowSensorCbDescr;

  const PidSettings m_pidSettings;

  std::atomic<millidegrees_t> m_targetTemp = 0;
  std::atomic<micrograms_t> m_ffWeight = 0;
};

extern template class PidTemperatureController<true>;
extern template class PidTemperatureController<false>;

} // namespace libopenpresso

#endif // CONTROLLERS_PID_TEMPERATURE_CONTROLLER_PID_TEMPERATURE_CONTROLLER_HPP