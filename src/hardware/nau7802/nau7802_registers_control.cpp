#include "nau7802_registers_control.hpp"

#include "nau7802_config.hpp"

#include <array>
#include <cerrno>
#include <cstdint>
#include <memory>
#include <mutex>
#include <span>

#include <libopenpresso/types.hpp>

#include <i2c/i2c_bus.hpp>

using namespace libopenpresso::nau7802;

RegistersControl::RegistersControl(const std::shared_ptr<i2c::I2cBus>& bus, i2c_dev_addr_t dev)
: m_bus{bus}
, m_dev{dev}
{
}

int32_t RegistersControl::readConversionResult()
{
  const auto addrVal = static_cast<uint8_t>(RegisterAddress::ConversionResult);
  std::array<uint8_t, WEIGHT_REGISTER_SIZE> conversionResultBuf{};
  readRaw(addrVal, conversionResultBuf);

  return ((static_cast<int32_t>(conversionResultBuf[0]) << 24) |
          (static_cast<int32_t>(conversionResultBuf[1]) << 16) |
          (static_cast<int32_t>(conversionResultBuf[2]) << 8)) >>
         8;
}

void RegistersControl::resetRegisters()
{
  writeRegisters(PowerUpRegisterData{.reset = RegisterReset::Reset});
}

bool RegistersControl::isAnalogPowerUp()
{
  PowerUpRegisterData pur;
  readRegisters(pur);
  return pur.analogPower == AnalogCircuitPower::PowerUp;
}

void RegistersControl::readRaw(uint8_t startAddr, std::span<uint8_t> data)
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);
  m_bus->write({&startAddr, &startAddr + 1});
  m_bus->read(data);
}

void RegistersControl::writeRaw(std::span<const uint8_t> data)
{
  std::scoped_lock lock(*m_bus);
  m_bus->setAddr(m_dev);
  m_bus->write(data);
}
