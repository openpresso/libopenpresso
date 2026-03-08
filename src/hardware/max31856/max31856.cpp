#include "max31856.hpp"

#include "max31856_config.hpp"

#include <array>
#include <bit>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <span>
#include <string>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

using namespace libopenpresso::max31856;

Max31856::Max31856(const unix_dev_addr_t& spiDev)
: m_devFd(open(spiDev.c_str(), O_RDWR))
, m_spiDev(spiDev)
{
  static const uint8_t SPI_MODE = SPI_MODE_1;
  if (ioctl(m_devFd.get(), SPI_IOC_WR_MODE, &SPI_MODE) < 0) {
    throw libopenpresso::SystemError{"Failed to set spi device mode"};
  }
  if (ioctl(m_devFd.get(), SPI_IOC_WR_BITS_PER_WORD, &SPI_BITS) < 0) {
    throw libopenpresso::SystemError{"Failed to set spi device word size"};
  }
  if (ioctl(m_devFd.get(), SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED) < 0) {
    throw libopenpresso::SystemError{"Failed to set spi device data rate"};
  }
}

template <RegisterAddress addr, size_t bytes>
void Max31856::writeRegister(std::span<const uint8_t, bytes> data) const
{
  static constexpr uint8_t addrByte = static_cast<uint8_t>(addr) | (1 << 7);
  std::array<spi_ioc_transfer, 2> tr{};

  tr[0].tx_buf = reinterpret_cast<uintptr_t>(&addrByte); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  tr[0].rx_buf = 0;
  tr[0].len = 1;
  tr[0].speed_hz = SPI_SPEED;
  tr[0].bits_per_word = SPI_BITS;

  tr[1].tx_buf = reinterpret_cast<uintptr_t>(data.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  tr[1].rx_buf = 0;
  tr[1].len = data.size();
  tr[1].speed_hz = SPI_SPEED;
  tr[1].bits_per_word = SPI_BITS;

  if (ioctl(m_devFd.get(), SPI_IOC_MESSAGE(2), &tr) < 0) {
    throw libopenpresso::SystemError{"Failed to read register"};
  }
}

template <RegisterAddress addr, size_t bytes>
void libopenpresso::max31856::Max31856::readRegister(std::span<uint8_t, bytes> data) const
{
  static constexpr auto addrByte = static_cast<uint8_t>(addr);
  std::array<spi_ioc_transfer, 2> tr{};

  tr[0].tx_buf = reinterpret_cast<uintptr_t>(&addrByte); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  tr[0].rx_buf = 0;
  tr[0].len = 1;
  tr[0].speed_hz = SPI_SPEED;
  tr[0].bits_per_word = SPI_BITS;

  tr[1].tx_buf = 0;
  tr[1].rx_buf = reinterpret_cast<uintptr_t>(data.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  tr[1].len = data.size();
  tr[1].speed_hz = SPI_SPEED;
  tr[1].bits_per_word = SPI_BITS;

  if (ioctl(m_devFd.get(), SPI_IOC_MESSAGE(2), &tr) < 0) {
    throw libopenpresso::SystemError{"Failed to read register"};
  }
}

ConfigData Max31856::readConfig() const
{
  std::array<uint8_t, 2> configBuf{};
  readRegister<RegisterAddress::Config0>(std::span{configBuf});

  return ConfigData::decode(configBuf);
}

void Max31856::writeConfig(ConfigData config) const
{
  const auto codedConfigBuf = config.encode();
  writeRegister<RegisterAddress::Config0>(std::span{codedConfigBuf});
}

void libopenpresso::max31856::Max31856::resetConfig() const
{
  writeConfig({});
}

uint8_t libopenpresso::max31856::Max31856::readFaultMask() const
{
  uint8_t result = 0;
  readRegister<RegisterAddress::FaultMask>(std::span{&result, 1});
  return 0;
}

void libopenpresso::max31856::Max31856::writeFaultMask(uint8_t bits) const
{
  writeRegister<RegisterAddress::FaultMask>(std::span<const uint8_t>{&bits, 1});
}

int16_t libopenpresso::max31856::Max31856::readCjHighThreshold() const
{
  std::array<uint8_t, 2> buf{};
  readRegister<RegisterAddress::CjHighFaultThreshold>(std::span{buf});
  return (buf[0] << 8) | buf[1];
}

void libopenpresso::max31856::Max31856::writeCjHighThreshold(int16_t temp) const
{
  std::array<uint8_t, 2> buf{};
  buf[0] = static_cast<uint8_t>(temp >> 8);
  buf[1] = static_cast<uint8_t>(temp & 0xff);
  writeRegister<RegisterAddress::CjHighFaultThreshold>(std::span<const uint8_t>{buf});
}

int16_t libopenpresso::max31856::Max31856::readCjLowThreshold() const
{
  std::array<uint8_t, 2> buf{};
  readRegister<RegisterAddress::CjLowFaultThreshold>(std::span{buf});
  return (buf[0] << 8) | buf[1];
}

void libopenpresso::max31856::Max31856::writeCjLowThreshold(int16_t temp) const
{
  std::array<uint8_t, 2> buf{};
  buf[0] = static_cast<uint8_t>(temp >> 8);
  buf[1] = static_cast<uint8_t>(temp & 0xff);
  writeRegister<RegisterAddress::CjLowFaultThreshold>(std::span<const uint8_t>{buf});
}

int16_t libopenpresso::max31856::Max31856::readThermocoupleHighThreshold() const
{
  std::array<uint8_t, 2> buf{};
  readRegister<RegisterAddress::TemperatureHighFaultThresholdB1>(std::span{buf});
  return (buf[0] << 8) | buf[1];
}

void libopenpresso::max31856::Max31856::writeThermocoupleHighThreshold(int16_t temp) const
{
  std::array<uint8_t, 2> buf{};
  buf[0] = static_cast<uint8_t>(temp >> 8);
  buf[1] = static_cast<uint8_t>(temp & 0xff);
  writeRegister<RegisterAddress::TemperatureHighFaultThresholdB1>(std::span<const uint8_t>{buf});
}

int16_t libopenpresso::max31856::Max31856::readThermocoupleLowThreshold() const
{
  std::array<uint8_t, 2> buf{};
  readRegister<RegisterAddress::TemperatureLowFaultThresholdB1>(std::span{buf});
  return (buf[0] << 8) | buf[1];
}

void libopenpresso::max31856::Max31856::writeThermocoupleLowThreshold(int16_t temp) const
{
  std::array<uint8_t, 2> buf{};
  buf[0] = static_cast<uint8_t>(temp >> 8);
  buf[1] = static_cast<uint8_t>(temp & 0xff);
  writeRegister<RegisterAddress::TemperatureLowFaultThresholdB1>(std::span<const uint8_t>{buf});
}

int8_t libopenpresso::max31856::Max31856::readCjOffset() const
{
  uint8_t result = 0;
  readRegister<RegisterAddress::CjTemperatureOffset>(std::span{&result, 1});
  return result;
}

void libopenpresso::max31856::Max31856::writeCjOffset(int8_t temp) const
{
  uint8_t writeByte = temp;
  writeRegister<RegisterAddress::CjTemperatureOffset>(std::span<const uint8_t>{&writeByte, 1});
}

int16_t libopenpresso::max31856::Max31856::readCjTemperature() const
{
  std::array<uint8_t, 2> buf{};
  readRegister<RegisterAddress::CjTemperatureB1>(std::span{buf});
  return static_cast<int16_t>((buf[0] << 8) | buf[1]) >> 2;
}

void libopenpresso::max31856::Max31856::writeCjTemperature(int16_t temp) const
{
  auto bitWidth = temp < 0 ? std::bit_width(static_cast<uint16_t>(-temp)) + 1
                           : std::bit_width(static_cast<uint16_t>(temp));

  if (bitWidth > CJ_TEMPERATURE_BIT_WIDTH) {
    throw libopenpresso::Exception{
      "CJ temperature {} is out of range (max {} bits)", temp, CJ_TEMPERATURE_BIT_WIDTH};
  }

  temp <<= 2;
  std::array<uint8_t, 2> buf{};
  buf[0] = static_cast<uint8_t>(temp >> 8);
  buf[1] = static_cast<uint8_t>(temp & 0xff);
  writeRegister<RegisterAddress::CjTemperatureB1>(std::span<const uint8_t>{buf});
}

int32_t Max31856::readThermocoupleTemperature() const
{
  std::array<uint8_t, THERMO_COUPLE_TEMPERATURE_REGISTER_SIZE> tempBuf{};
  readRegister<RegisterAddress::TemperatureB2>(std::span{tempBuf});

  return static_cast<int32_t>((tempBuf[0] << 16) | (tempBuf[1] << 8) | tempBuf[2]) >>
         THERMO_COUPLE_TEMPERATURE_BIT_SHIFT;
}

uint8_t libopenpresso::max31856::Max31856::readFaultStatusBits() const
{
  uint8_t result = 0;
  readRegister<RegisterAddress::FaultStatus>(std::span{&result, 1});
  return 0;
}

const std::string& Max31856::spiDev() const noexcept
{
  return m_spiDev;
}
