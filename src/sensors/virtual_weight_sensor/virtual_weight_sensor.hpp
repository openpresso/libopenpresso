#ifndef SENSORS_VIRTUAL_WEIGHT_SENSOR_VIRTUAL_WEIGHT_SENSOR_HPP
#define SENSORS_VIRTUAL_WEIGHT_SENSOR_VIRTUAL_WEIGHT_SENSOR_HPP

#include <atomic>
#include <chrono>
#include <memory>
#include <optional>
#include <unordered_map>

#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <utils/callback_descriptor_handler.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso
{

class PumpFlowSensor;

class VirtualWeightSensor : public interfaces::WeightSensor {
public:
  VirtualWeightSensor(const std::shared_ptr<PumpFlowSensor>& flow, time_delta_t flowRateSmoothingTime);

  void tare() override;
  milligrams_t getWeight() const override;
  milligrams_p_second_t getFlowRate() const override;
  std::optional<time_delta_t> fixedUpdateRate() const noexcept override;
  callback_descriptor_t registerCallback(const callback_t& callback) override;
  void unregisterCallback(callback_descriptor_t descr) override;

private:
  auto getFixedRateCallback(std::chrono::duration<float> dt, std::chrono::duration<float> smoothing);
  auto getVariableRateCallback(std::chrono::duration<float> smoothing);
  void processTareFlag(micrograms_t& weight, auto& dFilter);
  void storeAndNotify(milligrams_t weight, milligrams_p_second_t rate);

private:
  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, callback_t> m_callbacks;

  std::chrono::steady_clock::time_point m_prevTime;
  std::atomic<milligrams_t> m_weight = 0;
  std::atomic<milligrams_p_second_t> m_flowRate = 0;
  const std::optional<time_delta_t> m_fixedDt;
  std::atomic<bool> m_tareFlag = false;

  std::optional<CallbackDescriptorHandler> m_flowCallback;
};

} // namespace libopenpresso

#endif // SENSORS_VIRTUAL_WEIGHT_SENSOR_VIRTUAL_WEIGHT_SENSOR_HPP