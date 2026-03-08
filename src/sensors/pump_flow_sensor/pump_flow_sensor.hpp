#ifndef SENSORS_PUMP_FLOW_SENSOR_PUMP_FLOW_SENSOR_HPP
#define SENSORS_PUMP_FLOW_SENSOR_PUMP_FLOW_SENSOR_HPP

#include <atomic>
#include <cstddef>
#include <functional>
#include <optional>
#include <unordered_map>

#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pulse_controller.hpp>
#include <utils/callback_descriptor_handler.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso
{

namespace interfaces
{
class PressureSensor;
}

class PumpFlowSensor : public gpio::PulseCounter {
public:
  using callback_t = std::function<void(micrograms_t)>;
  PumpFlowSensor(const PressureSensorPtr& pressure, millibars_t pumpStallPressure, micrograms_t volumePerPulse);

  std::optional<time_delta_t> fixedUpdateRate() const noexcept;
  callback_descriptor_t registerCallback(const callback_t& callback);
  void unregisterCallback(callback_descriptor_t descr);

  void countPulse() noexcept override;

private:
  void pressureCallback(millibars_t pressure);

private:
  const millibars_t m_pumpStallPressure;
  const micrograms_t m_valuePerPulse;

  std::atomic<size_t> m_pulses = 0;

  PressureSensorPtr m_pressureSensor;
  CallbackDescriptorHandler m_pressureCallback;

  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, callback_t> m_callbacks;
};

} // namespace libopenpresso

#endif // SENSORS_PUMP_FLOW_SENSOR_PUMP_FLOW_SENSOR_HPP