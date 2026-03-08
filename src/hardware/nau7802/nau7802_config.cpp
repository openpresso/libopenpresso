#include "nau7802_config.hpp"

#include "nau7802_config_fields.hpp"

#include <cstdint>

using namespace libopenpresso::nau7802;

namespace libopenpresso::nau7802
{
namespace
{

template <typename T>
void parseField(T& val, uint8_t src)
{
  static constexpr uint8_t bitmask = (1 << (CONFIG_FIELD_POSITION<T>.bitsCount)) - 1;
  static constexpr uint8_t offset = CONFIG_FIELD_POSITION<T>.bitOffset;
  val = static_cast<T>((src >> offset) & bitmask);
}

template <typename T>
void writeField(T val, uint8_t& dst)
{
  static constexpr uint8_t bitmask = (1 << (CONFIG_FIELD_POSITION<T>.bitsCount)) - 1;
  static constexpr uint8_t offset = CONFIG_FIELD_POSITION<T>.bitOffset;
  dst |= (static_cast<uint8_t>(val) & bitmask) << offset;
}

} // namespace
} // namespace libopenpresso::nau7802

void PowerUpRegisterData::encode(uint8_t& dst) const noexcept
{
  dst = 0;
  writeField(voltageSelect, dst);
  writeField(clockSelect, dst);
  writeField(cycleStart, dst);
  writeField(analogPower, dst);
  writeField(digitalPower, dst);
  writeField(reset, dst);
}

PowerUpRegisterData PowerUpRegisterData::decode(uint8_t src) noexcept
{
  PowerUpRegisterData result;
  parseField(result.voltageSelect, src);
  parseField(result.clockSelect, src);
  parseField(result.cycleReady, src);
  parseField(result.powerReady, src);
  parseField(result.analogPower, src);
  parseField(result.digitalPower, src);
  return result;
}

void Control1RegisterData::encode(uint8_t& dst) const noexcept
{
  dst = 0;
  writeField(conversionReadyPinPolarity, dst);
  writeField(conversionReadyPinFunction, dst);
  writeField(ldoVoltage, dst);
  writeField(gainSelect, dst);
}

Control1RegisterData Control1RegisterData::decode(uint8_t src) noexcept
{
  Control1RegisterData result;
  parseField(result.conversionReadyPinPolarity, src);
  parseField(result.conversionReadyPinFunction, src);
  parseField(result.ldoVoltage, src);
  parseField(result.gainSelect, src);
  return result;
}

void Control2RegisterData::encode(uint8_t& dst) const noexcept
{
  dst = 0;
  writeField(channelSelect, dst);
  writeField(rateSelect, dst);
  writeField(calibrationResult, dst);
  writeField(calibrationControl, dst);
  writeField(calibrationMode, dst);
}

Control2RegisterData Control2RegisterData::decode(uint8_t src) noexcept
{
  Control2RegisterData result;
  parseField(result.channelSelect, src);
  parseField(result.rateSelect, src);
  parseField(result.calibrationResult, src);
  parseField(result.calibrationControl, src);
  parseField(result.calibrationMode, src);
  return result;
}

void PgaRegisterData::encode(uint8_t& dst) const noexcept
{
  dst = 0;
  writeField(readOtpSelect, dst);
  writeField(ldoMode, dst);
  writeField(pgaBuf, dst);
  writeField(pgaBypass, dst);
  writeField(pgaInvert, dst);
  writeField(pgaChopper, dst);
}

PgaRegisterData libopenpresso::nau7802::PgaRegisterData::decode(uint8_t src) noexcept
{
  PgaRegisterData result;
  parseField(result.readOtpSelect, src);
  parseField(result.ldoMode, src);
  parseField(result.pgaBuf, src);
  parseField(result.pgaBypass, src);
  parseField(result.pgaInvert, src);
  parseField(result.pgaChopper, src);
  return result;
}

void PowerRegisterData::encode(uint8_t& dst) const noexcept
{
  dst = 0;
  writeField(pgaCap, dst);
  writeField(masterBiasCurr, dst);
  writeField(adcCurr, dst);
  writeField(pgaCurr, dst);
}

void AdcRegisterData::encode(uint8_t& dst) const noexcept
{
  dst = 0;
  writeField(chopperClock, dst);
}

AdcRegisterData AdcRegisterData::decode(uint8_t src) noexcept
{
  AdcRegisterData result;
  parseField(result.chopperClock, src);
  return result;
}
