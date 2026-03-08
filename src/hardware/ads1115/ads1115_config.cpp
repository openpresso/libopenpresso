#include "ads1115_config.hpp"

#include "ads1115_config_fields.hpp"

#include <algorithm>
#include <cstdint>
#include <span>

using namespace libopenpresso::ads1115;

namespace libopenpresso::ads1115
{
namespace
{

template <typename T>
void parseField(T& val, std::span<const uint8_t, ConfigRegisterData::REGISTER_SIZE> src)
{
  static constexpr uint8_t bitmask = (1 << (CONFIG_FIELD_POSITION<T>.bitsCount)) - 1;
  static constexpr uint8_t offset = CONFIG_FIELD_POSITION<T>.bitOffset;
  static constexpr uint8_t index = CONFIG_FIELD_POSITION<T>.bytePosition;
  val = static_cast<T>((src[index] >> offset) & bitmask);
}

template <typename T>
void writeField(T val, std::span<uint8_t, ConfigRegisterData::REGISTER_SIZE> dst)
{
  static constexpr uint8_t bitmask = (1 << (CONFIG_FIELD_POSITION<T>.bitsCount)) - 1;
  static constexpr uint8_t offset = CONFIG_FIELD_POSITION<T>.bitOffset;
  static constexpr uint8_t index = CONFIG_FIELD_POSITION<T>.bytePosition;
  dst[index] |= (static_cast<uint8_t>(val) & bitmask) << offset;
}

} // namespace
} // namespace libopenpresso::ads1115

void ConfigRegisterData::encode(std::span<uint8_t, REGISTER_SIZE> dst) const noexcept
{
  std::ranges::fill(dst, 0);
  writeField(operationStatus, dst);
  writeField(inputMultiplexer, dst);
  writeField(programmableGainAmplifier, dst);
  writeField(operatingMode, dst);
  writeField(dataRate, dst);
  writeField(comparatorMode, dst);
  writeField(comparatorPolarity, dst);
  writeField(comparatorLatching, dst);
  writeField(comparatorQueue, dst);
}

ConfigRegisterData ConfigRegisterData::decode(std::span<const uint8_t, REGISTER_SIZE> src) noexcept
{
  ConfigRegisterData result;
  parseField(result.operationStatus, src);
  parseField(result.inputMultiplexer, src);
  parseField(result.programmableGainAmplifier, src);
  parseField(result.operatingMode, src);
  parseField(result.dataRate, src);
  parseField(result.comparatorMode, src);
  parseField(result.comparatorPolarity, src);
  parseField(result.comparatorLatching, src);
  parseField(result.comparatorQueue, src);
  return result;
}
