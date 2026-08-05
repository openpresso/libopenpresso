#ifndef SENSORS_NAU7802_WEIGHT_SENSOR_NAU7802_I2C_CONTROL_HPP
#define SENSORS_NAU7802_WEIGHT_SENSOR_NAU7802_I2C_CONTROL_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>

#include <libopenpresso/types.hpp>

#include <nau7802/nau7802_config.hpp>
#include <nau7802/nau7802_registers_control.hpp>

namespace libopenpresso
{

class Nau7802WeightSensorI2cControl {
public:
  static constexpr auto AVDD_SOURCE = nau7802::AvddSourceSelect::InternalLdo;
  static constexpr auto LDO_VOLTAGE = nau7802::LdoVoltage::_3_3;
  static constexpr auto PGA_MODE = nau7802::Gain::x128;
  static constexpr auto CONVERSION_RATE = nau7802::ConversionRate::sps10;

  template <typename F>
  Nau7802WeightSensorI2cControl(const std::shared_ptr<i2c::I2cBus>& bus, i2c_dev_addr_t dev, F&& afterInit)
  : m_control{bus, dev}
  , m_init{std::async(std::launch::async, [this, after = std::forward<F>(afterInit)] {
    initSequense(m_exit.get_future());
    std::invoke(std::move(after));
  })}
  {
  }

  Nau7802WeightSensorI2cControl(const Nau7802WeightSensorI2cControl&) = delete;
  Nau7802WeightSensorI2cControl(Nau7802WeightSensorI2cControl&&) = delete;
  auto operator=(const Nau7802WeightSensorI2cControl&) = delete;
  auto operator=(Nau7802WeightSensorI2cControl&&) = delete;

  ~Nau7802WeightSensorI2cControl();
  int32_t readRawWeight();
  void resetZero();
  void joinInitThread();

  static constexpr size_t sps() noexcept
  {
    // NOLINTBEGIN(readability-magic-numbers)
    switch (CONVERSION_RATE) {
    case nau7802::ConversionRate::sps10:
      return 10;
    case nau7802::ConversionRate::sps20:
      return 20;
    case nau7802::ConversionRate::sps40:
      return 40;
    case nau7802::ConversionRate::sps80:
      return 80;
    case nau7802::ConversionRate::sps320:
      return 320;
    }
    return 0;
    // NOLINTEND(readability-magic-numbers)
  }

private:
  void initSequense(const std::future<void>& exit);
  void runCalibration(const std::future<void>& exit, nau7802::CalibrationMode mode);
  void flushSamples(const std::future<void>& exit, size_t count);
  void flushSamples(size_t count);
  void resetChip(const std::future<void>& exit);
  void enableDigital(const std::future<void>& exit);
  void enableAnalog(const std::future<void>& exit);

private:
  nau7802::RegistersControl m_control;
  std::promise<void> m_exit;
  std::future<void> m_init;
};

} // namespace libopenpresso

#endif // SENSORS_NAU7802_WEIGHT_SENSOR_NAU7802_I2C_CONTROL_HPP