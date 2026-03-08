#ifndef SENSORS_MAX6675_TEMPERATURE_SENSOR_MAX6675_TEMPERATURE_SENSOR_HPP
#define SENSORS_MAX6675_TEMPERATURE_SENSOR_MAX6675_TEMPERATURE_SENSOR_HPP

#include <atomic>
#include <future>
#include <optional>
#include <thread>
#include <unordered_map>

#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <max6675/max6675.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso
{

class Max6675TemperatureSensor : public interfaces::TemperatureSensor {
  static constexpr auto SENSOR_UPDATE_TIMEOUT_MILLIS = std::chrono::milliseconds{250};

public:
  Max6675TemperatureSensor(const unix_dev_addr_t& spiDev);
  Max6675TemperatureSensor(const Max6675TemperatureSensor&) = delete;
  Max6675TemperatureSensor(Max6675TemperatureSensor&&) = delete;
  auto operator=(const Max6675TemperatureSensor&) = delete;
  auto operator=(Max6675TemperatureSensor&&) = delete;
  ~Max6675TemperatureSensor();

  millidegrees_t getTemperature() const override;
  std::optional<time_delta_t> fixedUpdateRate() const noexcept override;
  callback_descriptor_t registerCallback(const callback_t& callback) override;
  void unregisterCallback(callback_descriptor_t descr) override;

private:
  void worker(std::future<void> exit);

private:
  std::atomic<millidegrees_t> m_result = 0;
  Max6675 m_adc;

  spinlock m_callbacksLock;
  callback_descriptor_t m_nextCallbackDescriptor = 0;
  std::unordered_map<callback_descriptor_t, callback_t> m_callbacks;

  std::promise<void> m_exit;
  std::thread m_monitorThread;
};

} // namespace libopenpresso

#endif // SENSORS_MAX6675_TEMPERATURE_SENSOR_MAX6675_TEMPERATURE_SENSOR_HPP