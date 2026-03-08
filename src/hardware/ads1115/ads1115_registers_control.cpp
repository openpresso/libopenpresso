#include "ads1115_registers_control.hpp"

#include "ads1115_config.hpp"

#include <array>
#include <bit>
#include <cerrno>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>

#include <libopenpresso/types.hpp>

#include <i2c/i2c_bus.hpp>

using namespace libopenpresso::ads1115;

RegistersControl::RegistersControl(const std::shared_ptr<i2c::I2cBus>& bus, i2c_dev_addr_t dev)
: m_bus{bus}
, m_dev{dev}
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);

  const auto lastAddrVal = static_cast<uint8_t>(m_lastAddr);
  m_bus->write({&lastAddrVal, &lastAddrVal + 1});
}

int16_t RegistersControl::readConversionResult()
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);

  ensureReadRegisterAddr(RegisterAddress::ConversionRegister);

  std::array<uint8_t, TEMPERATURE_REGISTER_SIZE> conversionResultBuf{};
  m_bus->read(conversionResultBuf);

  return std::byteswap(std::bit_cast<int16_t>(conversionResultBuf));
}

void RegistersControl::resetLatchingComparatorState()
{
  readConfig();
}

ConfigRegisterData RegistersControl::readConfig()
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);

  ensureReadRegisterAddr(RegisterAddress::ConfigRegister);

  std::array<uint8_t, ConfigRegisterData::REGISTER_SIZE> configBuf{};
  m_bus->read(configBuf);

  return ConfigRegisterData::decode(configBuf);
}

void RegistersControl::writeConfig(ConfigRegisterData config)
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);

  std::array<uint8_t, ConfigRegisterData::REGISTER_SIZE + 1> configData{};
  configData[0] = static_cast<uint8_t>(RegisterAddress::ConfigRegister);
  config.encode(std::span(configData).subspan<1, 2>());

  m_bus->write(configData);

  m_lastAddr = RegisterAddress::ConfigRegister;
}

void RegistersControl::resetConfig()
{
  writeConfig({});
}

void RegistersControl::writeLoThreshold(int16_t val)
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);
  doWriteLoThreshold(val);
}

void RegistersControl::writeHiThreshold(int16_t val)
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);
  doWriteHiThreshold(val);
}

void RegistersControl::setThresholdsForConversionReadyMode()
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);
  doWriteLoThreshold(std::numeric_limits<int16_t>::max());
  doWriteHiThreshold(std::numeric_limits<int16_t>::min());
}

void RegistersControl::resetThresholds()
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);
  doWriteLoThreshold(std::numeric_limits<int16_t>::min());
  doWriteHiThreshold(std::numeric_limits<int16_t>::max());
}

void RegistersControl::doWriteLoThreshold(int16_t val)
{
  std::array<uint8_t, TEMPERATURE_REGISTER_SIZE + 1> thresholdData{};
  thresholdData[0] = static_cast<uint8_t>(RegisterAddress::LoThreshRegister);
  thresholdData[1] = static_cast<uint8_t>(val >> 8);
  thresholdData[2] = static_cast<uint8_t>(val & 0xff);

  m_bus->write(thresholdData);

  m_lastAddr = RegisterAddress::LoThreshRegister;
}

void RegistersControl::doWriteHiThreshold(int16_t val)
{
  std::array<uint8_t, TEMPERATURE_REGISTER_SIZE + 1> thresholdData{};
  thresholdData[0] = static_cast<uint8_t>(RegisterAddress::HiThreshRegister);
  thresholdData[1] = static_cast<uint8_t>(val >> 8);
  thresholdData[2] = static_cast<uint8_t>(val & 0xff);

  m_bus->write(thresholdData);

  m_lastAddr = RegisterAddress::HiThreshRegister;
}

void RegistersControl::ensureReadRegisterAddr(RegisterAddress addr)
{
  if (m_lastAddr == addr) {
    return;
  }

  const auto addrVal = static_cast<uint8_t>(addr);
  m_bus->write({&addrVal, &addrVal + 1});

  m_lastAddr = addr;
}
