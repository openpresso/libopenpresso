#ifndef HARDWARE_ADS1115_ADS1115_REGISTERS_CONTROL_HPP
#define HARDWARE_ADS1115_ADS1115_REGISTERS_CONTROL_HPP

#include "ads1115_config.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

#include <libopenpresso/types.hpp>

namespace libopenpresso::i2c
{
class I2cBus;
}

namespace libopenpresso::ads1115
{

class RegistersControl {
  static constexpr size_t TEMPERATURE_REGISTER_SIZE = 2;

public:
  RegistersControl(const std::shared_ptr<i2c::I2cBus>& bus, i2c_dev_addr_t dev);

  int16_t readConversionResult();
  void resetLatchingComparatorState();

  ConfigRegisterData readConfig();
  void writeConfig(ConfigRegisterData config);
  void resetConfig();

  void writeLoThreshold(int16_t val);
  void writeHiThreshold(int16_t val);
  void setThresholdsForConversionReadyMode();
  void resetThresholds();

private:
  void doWriteLoThreshold(int16_t val);
  void doWriteHiThreshold(int16_t val);
  void ensureReadRegisterAddr(RegisterAddress addr);

private:
  std::shared_ptr<i2c::I2cBus> m_bus;
  i2c_dev_addr_t m_dev;
  RegisterAddress m_lastAddr = RegisterAddress::ConversionRegister;
};

} // namespace libopenpresso::ads1115

#endif // HARDWARE_ADS1115_ADS1115_REGISTERS_CONTROL_HPP
