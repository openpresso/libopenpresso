#ifndef SENSORS_ADS1115_PRESSURE_SENSOR_ADS1115_I2C_CONTROL_HPP
#define SENSORS_ADS1115_PRESSURE_SENSOR_ADS1115_I2C_CONTROL_HPP

#include <cstdint>
#include <memory>

#include <libopenpresso/types.hpp>

#include <ads1115/ads1115_config.hpp>
#include <ads1115/ads1115_registers_control.hpp>

namespace libopenpresso
{

class Ads1115PressureSensorI2cControl {
public:
  static constexpr auto PGA_MODE = ads1115::PGAConfiguration::V6_144;
  static constexpr auto CONVERSION_RATE = ads1115::DataRate::SPS64;

  Ads1115PressureSensorI2cControl(const std::shared_ptr<i2c::I2cBus>& bus, i2c_dev_addr_t dev);
  Ads1115PressureSensorI2cControl(const Ads1115PressureSensorI2cControl&) = delete;
  Ads1115PressureSensorI2cControl(Ads1115PressureSensorI2cControl&&) = delete;
  auto operator=(const Ads1115PressureSensorI2cControl&) = delete;
  auto operator=(Ads1115PressureSensorI2cControl&&) = delete;
  ~Ads1115PressureSensorI2cControl();
  int16_t readRawPressure();

private:
  ads1115::RegistersControl m_control;
};

} // namespace libopenpresso

#endif // SENSORS_ADS1115_PRESSURE_SENSOR_ADS1115_I2C_CONTROL_HPP