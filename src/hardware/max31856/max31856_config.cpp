#include "max31856_config.hpp"

#include "max31856_config_fields.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <span>

using namespace libopenpresso::max31856;

namespace libopenpresso::max31856
{
namespace
{

template <typename T>
void parseField(T& val, std::span<const uint8_t, 2> src)
{
  static constexpr uint8_t bitmask = (1 << (CONFIG_FIELD_POSITION<T>.bitsCount)) - 1;
  static constexpr uint8_t offset = CONFIG_FIELD_POSITION<T>.bitOffset;
  static constexpr uint8_t index = CONFIG_FIELD_POSITION<T>.bytePosition;
  val = static_cast<T>((src[index] >> offset) & bitmask);
}

template <typename T>
void writeField(T val, std::span<uint8_t, 2> dst)
{
  static constexpr uint8_t bitmask = (1 << (CONFIG_FIELD_POSITION<T>.bitsCount)) - 1;
  static constexpr uint8_t offset = CONFIG_FIELD_POSITION<T>.bitOffset;
  static constexpr uint8_t index = CONFIG_FIELD_POSITION<T>.bytePosition;
  dst[index] |= (static_cast<uint8_t>(val) & bitmask) << offset;
}

} // namespace
} // namespace libopenpresso::max31856

void ConfigData::encode(std::span<uint8_t, 2> dst) const noexcept
{
  std::ranges::fill(dst, 0);
  writeField(conversionMode, dst);
  writeField(oneShotState, dst);
  writeField(ocDetectionTiming, dst);
  writeField(cjSensorState, dst);
  writeField(faultMode, dst);
  writeField(faultStatusClearBit, dst);
  writeField(noiseRejectionFilterFreq, dst);
  writeField(averaging, dst);
  writeField(tcType, dst);
}

std::array<uint8_t, 2> libopenpresso::max31856::ConfigData::encode() const noexcept
{
  std::array<uint8_t, 2> result = {0, 0};
  encode(result);
  return result;
}

ConfigData ConfigData::decode(std::span<const uint8_t, 2> src) noexcept
{
  ConfigData result;
  parseField(result.conversionMode, src);
  parseField(result.oneShotState, src);
  parseField(result.ocDetectionTiming, src);
  parseField(result.cjSensorState, src);
  parseField(result.faultMode, src);
  parseField(result.faultStatusClearBit, src);
  parseField(result.noiseRejectionFilterFreq, src);
  parseField(result.averaging, src);
  parseField(result.tcType, src);
  return result;
}
