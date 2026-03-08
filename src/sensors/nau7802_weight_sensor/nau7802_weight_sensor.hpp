#ifndef SENSORS_NAU7802_WEIGHT_SENSOR_NAU7802_WEIGHT_SENSOR_HPP
#define SENSORS_NAU7802_WEIGHT_SENSOR_NAU7802_WEIGHT_SENSOR_HPP

#include "nau7802_i2c_control.hpp"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>

#include <libopenpresso/interfaces/weight_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_monitor.hpp>
#include <nau7802/nau7802_registers_control.hpp>
#include <utils/derivative_filter.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso
{

namespace gpio
{
class PinMonitor;
}

class Nau7802WeightSensor : public interfaces::WeightSensor {
  static constexpr int WEIGHT_SCALE_BIT_OFFSET = 8;

public:
  Nau7802WeightSensor(const std::shared_ptr<i2c::I2cBus>& bus,
                      i2c_dev_addr_t dev,
                      const std::shared_ptr<gpio::PinMonitor>& monitor,
                      uint32_t scale,
                      time_delta_t flowRateSmoothingTime);
  Nau7802WeightSensor(const Nau7802WeightSensor&) = delete;
  Nau7802WeightSensor(Nau7802WeightSensor&&) = delete;
  auto operator=(const Nau7802WeightSensor&) = delete;
  auto operator=(Nau7802WeightSensor&&) = delete;
  ~Nau7802WeightSensor();

  void tare() override;
  milligrams_t getWeight() const override;
  milligrams_p_second_t getFlowRate() const override;
  std::optional<time_delta_t> fixedUpdateRate() const noexcept override;
  callback_descriptor_t registerCallback(const callback_t& cb) override;
  void unregisterCallback(callback_descriptor_t descr) override;

private:
  gpio::PinMonitor::PinEventCallback_t makeCallback();

private:
  const int32_t m_scale;
  std::atomic<milligrams_t> m_weight = 0;
  std::atomic<milligrams_p_second_t> m_rate = 0;
  std::atomic<bool> m_tareFlag = false;
  FilteredDerivative<float, true> m_dFiler;
  std::optional<gpio::PinMonitorCallbackHandler> m_monitorCallback;
  Nau7802WeightSensorI2cControl m_i2cControl;

  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, callback_t> m_callbacks;
};

} // namespace libopenpresso

#endif // SENSORS_NAU7802_WEIGHT_SENSOR_NAU7802_WEIGHT_SENSOR_HPP