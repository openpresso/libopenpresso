#ifndef CONTROLLERS_STEAM_CONTROLLER_STEAM_CONTROLLER_HPP
#define CONTROLLERS_STEAM_CONTROLLER_STEAM_CONTROLLER_HPP

#include <atomic>
#include <future>
#include <thread>

#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/interfaces/temperature_controller.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/types.hpp>

namespace libopenpresso
{

namespace interfaces
{
class TemperatureSensor;
class PressureSensor;
class TemperatureController;
class FlowRateController;
} // namespace interfaces

class SteamController final : public interfaces::TemperatureController {
public:
  SteamController(TemperatureSensorPtr temperatureSensor,
                  TemperatureControllerPtr preheatController,
                  TemperatureControllerPtr steamingTempertureController,
                  millidegrees_t temperatureThreshold,
                  PressureSensorPtr presureSensor,
                  millibars_t pressureThreshold,
                  FlowRateControllerPtr flowController,
                  milligrams_p_second_t refillFlow,
                  time_delta_t updatePeriod);

  SteamController(SteamController&&) = delete;
  SteamController(const SteamController&) = delete;
  auto operator=(SteamController&&) = delete;
  auto operator=(const SteamController&) = delete;
  ~SteamController();

  void activate() override;
  void deactivate() override;
  bool isActive() const noexcept override;
  millidegrees_t getTargetTemperature() const override;
  void setTargetTemperature(millidegrees_t millidegrees) override;

private:
  void worker(const std::future<void>& exit);
  void controlLoop(const std::future<void>& exit);
  bool isSteamValveOpen() const;

private:
  const TemperatureSensorPtr m_temperatureSensor;
  const TemperatureControllerPtr m_preheatController;
  const TemperatureControllerPtr m_steamingTemperatureController;
  const PressureSensorPtr m_presureSensor;
  const FlowRateControllerPtr m_flowController;
  const millibars_t m_pressureThreshold;
  const millidegrees_t m_temperatureThreshold;
  const milligrams_p_second_t m_refillFlow;

  std::atomic<millidegrees_t> m_steamTemperature = 0;
  std::promise<void> m_exit;
  std::thread m_workerThread;
  const time_delta_t m_updatePeriod;
  std::atomic<bool> m_isActive = false;
};

} // namespace libopenpresso

#endif // CONTROLLERS_STEAM_CONTROLLER_STEAM_CONTROLLER_HPP