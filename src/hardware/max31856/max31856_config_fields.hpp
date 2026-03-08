#ifndef HARDWARE_MAX31856_MAX31856_CONFIG_FIELDS_HPP
#define HARDWARE_MAX31856_MAX31856_CONFIG_FIELDS_HPP

#include "max31856_config.hpp"

#include <cstddef>
#include <type_traits>

namespace libopenpresso::max31856
{
struct ConfigFieldPosition_t {
  size_t bytePosition;
  size_t bitOffset;
  size_t bitsCount;
};

template <typename T>
static constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION = std::enable_if_t<false, T>{};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ConversionMode> = {
  .bytePosition = 0, .bitOffset = 7, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<OneShotState> = {
  .bytePosition = 0, .bitOffset = 6, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<OpenCircuitDetectionTiming> = {
  .bytePosition = 0, .bitOffset = 5, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<CjSensorState> = {
  .bytePosition = 0, .bitOffset = 4, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<FaultMode> = {
  .bytePosition = 0, .bitOffset = 2, .bitsCount = 2};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<FaultStatusClearBitState> = {
  .bytePosition = 0, .bitOffset = 1, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<NoiseRejectionFilterFreq> = {
  .bytePosition = 0, .bitOffset = 0, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<AveragingMode> = {
  .bytePosition = 1, .bitOffset = 4, .bitsCount = 3};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ThermocoupleType> = {
  .bytePosition = 1, .bitOffset = 0, .bitsCount = 4};

} // namespace libopenpresso::max31856

#endif // HARDWARE_MAX31856_MAX31856_CONFIG_FIELDS_HPP