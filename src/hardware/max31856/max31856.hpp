#ifndef HARDWARE_MAX31856_MAX31856_HPP
#define HARDWARE_MAX31856_MAX31856_HPP

#include "max31856_config.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

#include <libopenpresso/types.hpp>

#include <utils/fd_wrapper.hpp>

namespace libopenpresso::max31856
{

class Max31856 {
  static constexpr uint32_t SPI_SPEED = 5'000'000;
  static constexpr uint8_t SPI_BITS = 8;
  static constexpr size_t THERMO_COUPLE_TEMPERATURE_REGISTER_SIZE = 3;
  static constexpr size_t THERMO_COUPLE_TEMPERATURE_BIT_SHIFT = 5;
  static constexpr int CJ_TEMPERATURE_BIT_WIDTH = 14;

public:
  Max31856(const unix_dev_addr_t& spiDev);

  ConfigData readConfig() const;
  void writeConfig(ConfigData config) const;
  void resetConfig() const;

  uint8_t readFaultMask() const;
  void writeFaultMask(uint8_t bits) const;

  int16_t readCjHighThreshold() const;
  void writeCjHighThreshold(int16_t temp) const;

  int16_t readCjLowThreshold() const;
  void writeCjLowThreshold(int16_t temp) const;

  int16_t readThermocoupleHighThreshold() const;
  void writeThermocoupleHighThreshold(int16_t temp) const;

  int16_t readThermocoupleLowThreshold() const;
  void writeThermocoupleLowThreshold(int16_t temp) const;

  int8_t readCjOffset() const;
  void writeCjOffset(int8_t temp) const;

  int16_t readCjTemperature() const;
  void writeCjTemperature(int16_t temp) const;

  int32_t readThermocoupleTemperature() const;
  uint8_t readFaultStatusBits() const;
  const std::string& spiDev() const noexcept;

private:
  template <RegisterAddress addr, size_t bytes>
  void writeRegister(std::span<const uint8_t, bytes> data) const;

  template <RegisterAddress addr, size_t bytes>
  void readRegister(std::span<uint8_t, bytes> data) const;

private:
  fd_wrapper m_devFd;
  std::string m_spiDev;
};

} // namespace libopenpresso::max31856

#endif // HARDWARE_MAX31856_MAX31856_HPP
