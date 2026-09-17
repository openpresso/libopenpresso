#ifndef SENSORS_ADS1115_PRESSURE_SENSOR_ADS1115_PRESSURE_SENSOR_HPP
#define SENSORS_ADS1115_PRESSURE_SENSOR_ADS1115_PRESSURE_SENSOR_HPP

#include "ads1115_i2c_control.hpp"

#include <atomic>
#include <memory>
#include <optional>
#include <unordered_map>

#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <ads1115/ads1115_registers_control.hpp>
#include <gpio/pin_monitor.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso
{

namespace gpio
{
class PinMonitor;
}

class Ads1115PressureSensor final
: public interfaces::PressureSensor
, private Ads1115PressureSensorI2cControl {
public:
  static constexpr float TRANSDUCER_MIN_VOLTS = 0.5f;
  static constexpr float TRANSDUCER_MAX_VOLTS = 4.5f;
  static constexpr float TRANSDUCER_MIN_BARS = 0.0f;
  static constexpr float TRANSDUCER_MAX_BARS = 12.0f;

public:
  Ads1115PressureSensor(const std::shared_ptr<i2c::I2cBus>& bus,
                        i2c_dev_addr_t dev,
                        const std::shared_ptr<gpio::PinMonitor>& monitor);
  Ads1115PressureSensor(const Ads1115PressureSensor&) = delete;
  Ads1115PressureSensor(Ads1115PressureSensor&&) = delete;
  auto operator=(const Ads1115PressureSensor&) = delete;
  auto operator=(Ads1115PressureSensor&&) = delete;
  ~Ads1115PressureSensor() = default;

  millibars_t getPressure() const override;
  std::optional<time_delta_t> fixedUpdateRate() const noexcept override;
  callback_descriptor_t registerCallback(const callback_t& callback) override;
  void unregisterCallback(callback_descriptor_t descr) override;

private:
  gpio::PinMonitor::PinEventCallback_t makeCallback();

private:
  std::atomic<millibars_t> m_millibars = 0;
  gpio::PinMonitorCallbackHandler m_monitorCallback;

  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, callback_t> m_callbacks;
};

} // namespace libopenpresso

#endif // SENSORS_ADS1115_PRESSURE_SENSOR_ADS1115_PRESSURE_SENSOR_HPP