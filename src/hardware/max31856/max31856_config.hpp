#ifndef HARDWARE_MAX31856_MAX31856_CONFIG_HPP
#define HARDWARE_MAX31856_MAX31856_CONFIG_HPP

#include <array>
#include <cstdint>
#include <span>

namespace libopenpresso::max31856
{

static constexpr uint8_t THERMOCOUPLE_DEGREE_BITS_SHIFT = 7;
static constexpr uint8_t CJ_DEGREE_BITS_SHIFT = 6;
static constexpr uint8_t CJ_OFFSET_DEGREE_BITS_SHIFT = 4;
static constexpr uint8_t THERMOCOUPLE_FAULT_DEGREE_BITS_SHIFT = 4;

enum class RegisterAddress : uint8_t {
  Config0 = 0x0,
  Config1 = 0x1,
  FaultMask = 0x2,
  CjHighFaultThreshold = 0x3,
  CjLowFaultThreshold = 0x4,
  TemperatureHighFaultThresholdB1 = 0x5,
  TemperatureHighFaultThresholdB0 = 0x6,
  TemperatureLowFaultThresholdB1 = 0x7,
  TemperatureLowFaultThresholdB0 = 0x8,
  CjTemperatureOffset = 0x9,
  CjTemperatureB1 = 0xA,
  CjTemperatureB0 = 0xB,
  TemperatureB2 = 0xC,
  TemperatureB1 = 0xD,
  TemperatureB0 = 0xE,
  FaultStatus = 0xF,
};

enum class ConversionMode : uint8_t {
  NormallyOff = 0,
  AutomaticConversion = 1,
};

enum class OneShotState : uint8_t {
  NoConversionRequested = 0,
  ConversionRequested = 1,
};

enum class OpenCircuitDetectionTiming : uint8_t {
  Disabled = 0,
  _15MS = 1,
  _37MS = 2,
  _125MS = 3,
};

enum class CjSensorState : uint8_t {
  Enabled = 0,
  Disabled = 1,
};

enum class FaultMode : uint8_t {
  Comparator = 0,
  Interrupt = 1,
};

enum class FaultStatusClearBitState : uint8_t {
  Default = 0,
  ClearRequested = 1,
};

enum class NoiseRejectionFilterFreq : uint8_t {
  _60Hz = 0,
  _50Hz = 1,
};

enum class AveragingMode : uint8_t {
  No = 0,
  _2Samples = 1,
  _4Samples = 2,
  _8Samples = 3,
  _16Samples = 4,
};

enum class ThermocoupleType : uint8_t {
  B = 0,
  E = 1,
  J = 2,
  K = 3,
  N = 4,
  R = 5,
  S = 6,
  T = 7,
};

enum class FaultMask : uint8_t {
  CjHigh = 1 << 5,
  CjLow = 1 << 4,
  TcHigh = 1 << 3,
  TcLow = 1 << 2,
  OverUnderVoltage = 1 << 1,
  OpenCircuit = 1 << 0,
};

enum class FaultStatus : uint8_t {
  CjRange = 1 << 7,
  TcRange = 1 << 6,
  CjHigh = 1 << 5,
  CjLow = 1 << 4,
  TcHigh = 1 << 3,
  TcLow = 1 << 2,
  OverUnderVoltage = 1 << 1,
  OpenCircuit = 1 << 0,
};

struct ConfigData {
  ConversionMode conversionMode = ConversionMode::NormallyOff;
  OneShotState oneShotState = OneShotState::NoConversionRequested;
  OpenCircuitDetectionTiming ocDetectionTiming = OpenCircuitDetectionTiming::Disabled;
  CjSensorState cjSensorState = CjSensorState::Enabled;
  FaultMode faultMode = FaultMode::Comparator;
  FaultStatusClearBitState faultStatusClearBit = FaultStatusClearBitState::Default;
  NoiseRejectionFilterFreq noiseRejectionFilterFreq = NoiseRejectionFilterFreq::_60Hz;
  AveragingMode averaging = AveragingMode::No;
  ThermocoupleType tcType = ThermocoupleType::K;

  void encode(std::span<uint8_t, 2> dst) const noexcept;
  std::array<uint8_t, 2> encode() const noexcept;
  static ConfigData decode(std::span<const uint8_t, 2> src) noexcept;
};

} // namespace libopenpresso::max31856

#endif // HARDWARE_MAX31856_MAX31856_CONFIG_HPP