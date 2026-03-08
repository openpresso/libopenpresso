#ifndef HARDWARE_NAU7802_NAU7802_CONFIG_HPP
#define HARDWARE_NAU7802_NAU7802_CONFIG_HPP

#include <cstdint>

namespace libopenpresso::nau7802
{

enum class RegisterAddress : uint8_t {
  PowerUpControl = 0x00,
  Control1 = 0x01,
  Control2 = 0x02,
  Channel1OffsetCalibration = 0x03,
  Channel1GainCalibration = 0x06,
  Channel2OffsetCalibration = 0x0A,
  Channel2GainCalibration = 0x0D,
  I2cControl = 0x11,
  ConversionResult = 0x12,
  AdcOtpRegisters = 0x15,
  PgaRegister = 0x1B,
  PowerControlRegister = 0x1C,
};

enum class AvddSourceSelect : uint8_t {
  AvddInputPin = 0,
  InternalLdo = 1,
};

enum class SystemClockSourceSelect : uint8_t {
  InternalRcOscillator = 0,
  ExternalCrystal = 1,
};

enum class CycleReady : uint8_t {
  NotReady = 0,
  AdcDataReady = 1,
};

enum class CycleStart : uint8_t {
  NotAction = 0,
  Start = 1,
};

enum class PowerUpReady : uint8_t {
  NotReady = 0,
  PowerUpReady = 1,
};

enum class AnalogCircuitPower : uint8_t {
  PowerDown = 0,
  PowerUp = 1,
};

enum class DigitalCircuitPower : uint8_t {
  PowerDown = 0,
  PowerUp = 1,
};

enum class RegisterReset : uint8_t {
  NormalOperation = 0,
  Reset = 1,
};

enum class ConversionReadyPolarity : uint8_t {
  HighActive = 0,
  LowActive = 1,
};

enum class ReadyPinFunction : uint8_t {
  ConversionReady = 0,
  ClockOutput = 1,
};

enum class LdoVoltage : uint8_t {
  _4_5 = 0x0,
  _4_2 = 0x01,
  _3_9 = 0x02,
  _3_6 = 0x03,
  _3_3 = 0x04,
  _3_0 = 0x05,
  _2_7 = 0x06,
  _2_4 = 0x07,
};

enum class Gain : uint8_t {
  x1 = 0x0,
  x2 = 0x01,
  x4 = 0x02,
  x8 = 0x03,
  x16 = 0x04,
  x32 = 0x05,
  x64 = 0x07,
  x128 = 0x08,
};

enum class AnalogInputChannel : uint8_t {
  Ch1 = 0,
  Ch2 = 1,
};

enum class ConversionRate : uint8_t {
  sps10 = 0x0,
  sps20 = 0x01,
  sps40 = 0x02,
  sps80 = 0x03,
  sps320 = 0x07,
};

enum class CalibrationResult : uint8_t {
  NoError = 0,
  CalibrationError = 1,
};

enum class CalibrationActionControl : uint8_t {
  NoAction = 0,
  StartCalibration = 1,
};

enum class CalibrationMode : uint8_t {
  DefaultCalibration = 0x0,
  Reserved = 0x01,
  OffsetCalibration = 0x02,
  GainCalibration = 0x03,
};

enum class ChopperClock : uint8_t {
  Reserved0 = 0x0,
  Reserved1 = 0x1,
  Reserved2 = 0x2,
  TurnedOff = 0x3,
};

enum class OtpRegSelect : uint8_t {
  ReadAdcReg = 0,
  ReadOtp = 1,
};

enum class LdoMode : uint8_t {
  Accuracy = 0,
  Stability = 1,
};

enum class PgaOutputBuf : uint8_t {
  Disable = 0,
  Enable = 1,
};

enum class PgaBypass : uint8_t {
  Disable = 0,
  Enable = 1,
};

enum class PgaInvert : uint8_t {
  Default = 0,
  Inverted = 1,
};

enum class PgaChopper : uint8_t {
  Default = 0,
  Disable = 1,
};

enum class PgaCapacitor : uint8_t {
  Disabled = 0,
  Enabled = 1,
};

enum class MasterBiasCurrent : uint8_t {
  _100 = 0x0,
  _90 = 0x01,
  _80 = 0x02,
  _73 = 0x03,
  _67 = 0x04,
  _62 = 0x05,
  _58 = 0x06,
  _54 = 0x07,
};

enum class AdcCurrent : uint8_t {
  _100 = 0x0,
  _75 = 0x1,
  _50 = 0x2,
  _25 = 0x3,
};

enum class PgaCurrent : uint8_t {
  _100 = 0x0,
  _95 = 0x1,
  _86 = 0x2,
  _70 = 0x3,
};

struct PowerUpRegisterData {
  static constexpr RegisterAddress addr = RegisterAddress::PowerUpControl;

  AvddSourceSelect voltageSelect = AvddSourceSelect::AvddInputPin;
  SystemClockSourceSelect clockSelect = SystemClockSourceSelect::InternalRcOscillator;
  CycleReady cycleReady = CycleReady::NotReady;
  CycleStart cycleStart = CycleStart::NotAction;
  PowerUpReady powerReady = PowerUpReady::NotReady;
  AnalogCircuitPower analogPower = AnalogCircuitPower::PowerDown;
  DigitalCircuitPower digitalPower = DigitalCircuitPower::PowerDown;
  RegisterReset reset = RegisterReset::NormalOperation;

  void encode(uint8_t& dst) const noexcept;
  static PowerUpRegisterData decode(uint8_t src) noexcept;
};

struct Control1RegisterData {
  static constexpr RegisterAddress addr = RegisterAddress::Control1;

  ConversionReadyPolarity conversionReadyPinPolarity = ConversionReadyPolarity::HighActive;
  ReadyPinFunction conversionReadyPinFunction = ReadyPinFunction::ConversionReady;
  LdoVoltage ldoVoltage = LdoVoltage::_4_5;
  Gain gainSelect = Gain::x1;

  void encode(uint8_t& dst) const noexcept;
  static Control1RegisterData decode(uint8_t src) noexcept;
};

struct Control2RegisterData {
  static constexpr RegisterAddress addr = RegisterAddress::Control2;

  AnalogInputChannel channelSelect = AnalogInputChannel::Ch1;
  ConversionRate rateSelect = ConversionRate::sps10;
  CalibrationResult calibrationResult = CalibrationResult::NoError;
  CalibrationActionControl calibrationControl = CalibrationActionControl::NoAction;
  CalibrationMode calibrationMode = CalibrationMode::DefaultCalibration;

  void encode(uint8_t& dst) const noexcept;
  static Control2RegisterData decode(uint8_t src) noexcept;
};

struct AdcRegisterData {
  static constexpr RegisterAddress addr = RegisterAddress::AdcOtpRegisters;

  ChopperClock chopperClock = ChopperClock::Reserved0;

  void encode(uint8_t& dst) const noexcept;
  static AdcRegisterData decode(uint8_t src) noexcept;
};

struct PgaRegisterData {
  static constexpr RegisterAddress addr = RegisterAddress::PgaRegister;

  OtpRegSelect readOtpSelect = OtpRegSelect::ReadOtp;
  LdoMode ldoMode = LdoMode::Accuracy;
  PgaOutputBuf pgaBuf = PgaOutputBuf::Disable;
  PgaBypass pgaBypass = PgaBypass::Disable;
  PgaInvert pgaInvert = PgaInvert::Default;
  PgaChopper pgaChopper = PgaChopper::Default;

  void encode(uint8_t& dst) const noexcept;
  static PgaRegisterData decode(uint8_t src) noexcept;
};

struct PowerRegisterData {
  static constexpr RegisterAddress addr = RegisterAddress::PowerControlRegister;

  PgaCapacitor pgaCap = PgaCapacitor::Disabled;
  MasterBiasCurrent masterBiasCurr = MasterBiasCurrent::_100;
  AdcCurrent adcCurr = AdcCurrent::_100;
  PgaCurrent pgaCurr = PgaCurrent::_100;

  void encode(uint8_t& dst) const noexcept;
  static PowerRegisterData decode(uint8_t src) noexcept;
};

} // namespace libopenpresso::nau7802

#endif // HARDWARE_NAU7802_NAU7802_CONFIG_HPP