#ifndef SENSORS_MAX31856_TEMPERATURE_SENSOR_MAX31856_SPI_CONTROL_HPP
#define SENSORS_MAX31856_TEMPERATURE_SENSOR_MAX31856_SPI_CONTROL_HPP

#include <cstdint>

#include <libopenpresso/types.hpp>

#include <max31856/max31856.hpp>

namespace libopenpresso
{

class Max31856TemperatureSensorSpiControl {
public:
  Max31856TemperatureSensorSpiControl(const unix_dev_addr_t& spiDev);
  Max31856TemperatureSensorSpiControl(const Max31856TemperatureSensorSpiControl&) = delete;
  Max31856TemperatureSensorSpiControl(Max31856TemperatureSensorSpiControl&&) = delete;
  auto operator=(const Max31856TemperatureSensorSpiControl&) = delete;
  auto operator=(Max31856TemperatureSensorSpiControl&&) = delete;
  ~Max31856TemperatureSensorSpiControl();
  int32_t readTemperature();

private:
  max31856::Max31856 m_control;
};

} // namespace libopenpresso

#endif // SENSORS_MAX31856_TEMPERATURE_SENSOR_MAX31856_SPI_CONTROL_HPP