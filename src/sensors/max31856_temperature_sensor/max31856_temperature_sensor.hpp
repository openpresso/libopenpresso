#ifndef SENSORS_MAX31856_TEMPERATURE_SENSOR_MAX31856_TEMPERATURE_SENSOR_HPP
#define SENSORS_MAX31856_TEMPERATURE_SENSOR_MAX31856_TEMPERATURE_SENSOR_HPP

#include "max31856_spi_control.hpp"

#include <atomic>
#include <memory>
#include <optional>
#include <unordered_map>

#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_monitor.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso
{

class Max31856TemperatureSensor final
: public interfaces::TemperatureSensor
, private Max31856TemperatureSensorSpiControl {
public:
  Max31856TemperatureSensor(const unix_dev_addr_t& spiDev, const std::shared_ptr<gpio::PinMonitor>& monitor);
  Max31856TemperatureSensor(const Max31856TemperatureSensor&) = delete;
  Max31856TemperatureSensor(Max31856TemperatureSensor&&) = delete;
  auto operator=(const Max31856TemperatureSensor&) = delete;
  auto operator=(Max31856TemperatureSensor&&) = delete;
  ~Max31856TemperatureSensor() = default;

  millidegrees_t getTemperature() const override;
  std::optional<time_delta_t> fixedUpdateRate() const noexcept override;
  callback_descriptor_t registerCallback(const callback_t& callback) override;
  void unregisterCallback(callback_descriptor_t descr) override;

private:
  gpio::PinMonitor::PinEventCallback_t makeCallback();

private:
  std::atomic<millidegrees_t> m_millidegrees = 0;
  gpio::PinMonitorCallbackHandler m_callback;

  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, callback_t> m_callbacks;
};

} // namespace libopenpresso

#endif // SENSORS_MAX31856_TEMPERATURE_SENSOR_MAX31856_TEMPERATURE_SENSOR_HPP