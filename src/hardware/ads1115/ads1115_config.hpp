#ifndef HARDWARE_ADS1115_ADS1115_CONFIG_HPP
#define HARDWARE_ADS1115_ADS1115_CONFIG_HPP

#include <cstddef>
#include <cstdint>
#include <span>

namespace libopenpresso::ads1115
{

enum class RegisterAddress : uint8_t {
  ConversionRegister = 0x0,
  ConfigRegister = 0x1,
  LoThreshRegister = 0x2,
  HiThreshRegister = 0x3,
};

enum class OperationalStatus : uint8_t {
  Performing_NoEffect = 0x0,
  Idle_StartConversion = 0x01,
};

enum class MultiplexerConfiguration : uint8_t {
  AIN0_AIN1 = 0x0,
  AIN0_AIN3 = 0x1,
  AIN1_AIN3 = 0x2,
  AIN2_AIN3 = 0x3,
  AIN0_GND = 0x4,
  AIN1_GND = 0x5,
  AIN2_GND = 0x6,
  AIN3_GND = 0x7,
};

enum class PGAConfiguration : uint8_t {
  V6_144 = 0x0,
  V4_096 = 0x1,
  V2_048 = 0x2,
  V1_024 = 0x3,
  V0_512 = 0x4,
  V0_256 = 0x5,
  V0_256_2 = 0x6,
  V0_256_3 = 0x7,
};

constexpr float voltsPerBit(PGAConfiguration pga) noexcept
{
  // NOLINTBEGIN(readability-magic-numbers)
  switch (pga) {
  case PGAConfiguration::V6_144:
    return 187.5f * 0.000'001f;
  case PGAConfiguration::V4_096:
    return 125.f * 0.000'001f;
  case PGAConfiguration::V2_048:
    return 62.5f * 0.000'001f;
  case PGAConfiguration::V1_024:
    return 31.25f * 0.000'001f;
  case PGAConfiguration::V0_512:
    return 15.625f * 0.000'001f;
  case PGAConfiguration::V0_256:
    [[fallthrough]];
  case PGAConfiguration::V0_256_2:
    [[fallthrough]];
  case PGAConfiguration::V0_256_3:
    return 7.8125f * 0.000'001f;
  default:
    break;
  }
  return 0.0f;
  // NOLINTEND(readability-magic-numbers)
}

enum class OperatingMode : uint8_t {
  Continuous = 0x0,
  SingleShot = 0x1,
};

enum class DataRate : uint8_t {
  SPS8 = 0x0,
  SPS16 = 0x1,
  SPS32 = 0x2,
  SPS64 = 0x3,
  SPS128 = 0x4,
  SPS250 = 0x5,
  SPS475 = 0x6,
  SPS860 = 0x7,
};

enum class ComparatorMode : uint8_t {
  Traditional = 0x0,
  Window = 0x1,
};

enum class ComparatorLatchType : uint8_t {
  NonLatching = 0x0,
  Latching = 0x1,
};

enum class ComparatorPolarity : uint8_t {
  ActiveLow = 0x0,
  ActiveHigh = 0x1,
};

enum class ComparatorQueueMode : uint8_t {
  AssertAfterOne = 0x0,
  AssertAfterTwo = 0x1,
  AssertAfterThree = 0x2,
  DisableComparator = 0x3,
};

struct ConfigRegisterData {
  static constexpr size_t REGISTER_SIZE = 2;

  OperationalStatus operationStatus = OperationalStatus::Idle_StartConversion; // Operational
                                                                               // status or
                                                                               // single-shot
                                                                               // conversion start
  MultiplexerConfiguration inputMultiplexer = MultiplexerConfiguration::AIN0_AIN1;
  PGAConfiguration programmableGainAmplifier = PGAConfiguration::V2_048;
  OperatingMode operatingMode = OperatingMode::SingleShot;
  DataRate dataRate = DataRate::SPS128;
  ComparatorMode comparatorMode = ComparatorMode::Traditional;
  ComparatorPolarity comparatorPolarity = ComparatorPolarity::ActiveLow;
  ComparatorLatchType comparatorLatching = ComparatorLatchType::NonLatching;
  ComparatorQueueMode comparatorQueue = ComparatorQueueMode::DisableComparator;

  void encode(std::span<uint8_t, REGISTER_SIZE> dst) const noexcept;
  static ConfigRegisterData decode(std::span<const uint8_t, REGISTER_SIZE> src) noexcept;
};

} // namespace libopenpresso::ads1115

#endif // HARDWARE_ADS1115_ADS1115_CONFIG_HPP
