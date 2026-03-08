#ifndef HARDWARE_NAU7802_NAU7802_CONFIG_FIELDS_HPP
#define HARDWARE_NAU7802_NAU7802_CONFIG_FIELDS_HPP

#include "nau7802_config.hpp"

#include <cstddef>
#include <type_traits>

namespace libopenpresso::nau7802
{

struct ConfigFieldPosition_t {
  size_t bitOffset;
  size_t bitsCount;
};

template <typename T>
static constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION = std::enable_if_t<false, T>{};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<AvddSourceSelect> = {.bitOffset = 7,
                                                                                  .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<SystemClockSourceSelect> = {
  .bitOffset = 6, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<CycleReady> = {.bitOffset = 5, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<CycleStart> = {.bitOffset = 4, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<PowerUpReady> = {.bitOffset = 3,
                                                                              .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<AnalogCircuitPower> = {.bitOffset = 2,
                                                                                    .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<DigitalCircuitPower> = {.bitOffset = 1,
                                                                                     .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<RegisterReset> = {.bitOffset = 0,
                                                                               .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ConversionReadyPolarity> = {
  .bitOffset = 7, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ReadyPinFunction> = {.bitOffset = 6,
                                                                                  .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<LdoVoltage> = {.bitOffset = 3, .bitsCount = 3};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<Gain> = {.bitOffset = 0, .bitsCount = 3};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<AnalogInputChannel> = {.bitOffset = 7,
                                                                                    .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ConversionRate> = {.bitOffset = 4,
                                                                                .bitsCount = 3};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<CalibrationResult> = {.bitOffset = 3,
                                                                                   .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<CalibrationActionControl> = {
  .bitOffset = 2, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<CalibrationMode> = {.bitOffset = 0,
                                                                                 .bitsCount = 2};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<ChopperClock> = {.bitOffset = 4,
                                                                              .bitsCount = 2};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<OtpRegSelect> = {.bitOffset = 7,
                                                                              .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<LdoMode> = {.bitOffset = 6, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<PgaOutputBuf> = {.bitOffset = 5,
                                                                              .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<PgaBypass> = {.bitOffset = 4, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<PgaInvert> = {.bitOffset = 4, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<PgaChopper> = {.bitOffset = 0, .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<PgaCapacitor> = {.bitOffset = 7,
                                                                              .bitsCount = 1};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<MasterBiasCurrent> = {.bitOffset = 4,
                                                                                   .bitsCount = 3};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<AdcCurrent> = {.bitOffset = 2, .bitsCount = 2};

template <>
constexpr inline ConfigFieldPosition_t CONFIG_FIELD_POSITION<PgaCurrent> = {.bitOffset = 0, .bitsCount = 2};

} // namespace libopenpresso::nau7802

#endif // HARDWARE_NAU7802_NAU7802_CONFIG_FIELDS_HPP