#ifndef HARDWARE_ADS1115_ADS1115_CONFIG_FIELDS_HPP
#define HARDWARE_ADS1115_ADS1115_CONFIG_FIELDS_HPP

#include "ads1115_config.hpp"

#include <cstddef>
#include <type_traits>

namespace libopenpresso::ads1115
{
struct ConfigFieldPosition_t {
  size_t bytePosition;
  size_t bitOffset;
  size_t bitsCount;
};

template <typename T>
static constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION = std::enable_if_t<false, T>{};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<OperationalStatus> = {
  .bytePosition = 0, .bitOffset = 7, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<MultiplexerConfiguration> = {
  .bytePosition = 0, .bitOffset = 4, .bitsCount = 3};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<PGAConfiguration> = {
  .bytePosition = 0, .bitOffset = 1, .bitsCount = 3};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<OperatingMode> = {
  .bytePosition = 0, .bitOffset = 0, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<DataRate> = {
  .bytePosition = 1, .bitOffset = 5, .bitsCount = 3};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ComparatorMode> = {
  .bytePosition = 1, .bitOffset = 4, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ComparatorPolarity> = {
  .bytePosition = 1, .bitOffset = 3, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ComparatorLatchType> = {
  .bytePosition = 1, .bitOffset = 2, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ComparatorQueueMode> = {
  .bytePosition = 1, .bitOffset = 0, .bitsCount = 2};

} // namespace libopenpresso::ads1115

#endif // HARDWARE_ADS1115_ADS1115_CONFIG_FIELDS_HPP