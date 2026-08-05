#include "nau7802_i2c_control.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <future>
#include <thread>

#include <nau7802/nau7802_config.hpp>

using namespace libopenpresso;
using namespace std::chrono_literals;

Nau7802WeightSensorI2cControl::~Nau7802WeightSensorI2cControl()
{
  try {
    m_control.resetRegisters();
  }
  catch (...) {
    std::abort();
  }
}

int32_t Nau7802WeightSensorI2cControl::readRawWeight()
{
  return m_control.readConversionResult();
}

void Nau7802WeightSensorI2cControl::resetZero()
{
  nau7802::Control2RegisterData calibRegister = {
    .rateSelect = CONVERSION_RATE,
    .calibrationControl = nau7802::CalibrationActionControl::StartCalibration,
    .calibrationMode = nau7802::CalibrationMode::OffsetCalibration,
  };

  m_control.writeRegisters(calibRegister);

  while (calibRegister.calibrationControl == nau7802::CalibrationActionControl::StartCalibration) {
    std::this_thread::sleep_for(10ms);
    m_control.readRegisters(calibRegister);
  }

  static constexpr size_t flushAfterOffsetCalib = 10;
  flushSamples(flushAfterOffsetCalib);
}

void Nau7802WeightSensorI2cControl::joinInitThread()
{
  m_exit.set_value();
  m_init.get();
}

void Nau7802WeightSensorI2cControl::initSequense(const std::future<void>& exit)
{
  static constexpr size_t flushAfterAnalogEnable = 10;
  static constexpr size_t flushAfterDefaultCalib = 6;
  static constexpr size_t flushAfterOffsetCalib = 10;

  resetChip(exit);
  enableDigital(exit);
  enableAnalog(exit);
  flushSamples(exit, flushAfterAnalogEnable);
  runCalibration(exit, nau7802::CalibrationMode::DefaultCalibration);
  flushSamples(exit, flushAfterDefaultCalib);
  runCalibration(exit, nau7802::CalibrationMode::OffsetCalibration);
  // runCalibration(exit, nau7802::CalibrationMode::GainCalibration);
  flushSamples(exit, flushAfterOffsetCalib);
}

void Nau7802WeightSensorI2cControl::runCalibration(const std::future<void>& exit,
                                                   nau7802::CalibrationMode mode)
{
  nau7802::Control2RegisterData calibRegister = {
    .rateSelect = CONVERSION_RATE,
    .calibrationControl = nau7802::CalibrationActionControl::StartCalibration,
    .calibrationMode = mode,
  };

  m_control.writeRegisters(calibRegister);

  while (calibRegister.calibrationControl == nau7802::CalibrationActionControl::StartCalibration) {
    if (exit.wait_for(100ms) == std::future_status::ready) {
      return;
    }
    m_control.readRegisters(calibRegister);
  }
}

void Nau7802WeightSensorI2cControl::flushSamples(const std::future<void>& exit, size_t count)
{
  auto startRegister = nau7802::PowerUpRegisterData{
    .voltageSelect = AVDD_SOURCE,
    .cycleStart = nau7802::CycleStart::Start,
    .analogPower = nau7802::AnalogCircuitPower::PowerUp,
    .digitalPower = nau7802::DigitalCircuitPower::PowerUp,
  };
  m_control.writeRegisters(startRegister);

  for (size_t i = 0; i < count; ++i) {
    while (startRegister.cycleReady == nau7802::CycleReady::NotReady) {
      if (exit.wait_for(10ms) == std::future_status::ready) {
        return;
      }
      m_control.readRegisters(startRegister);
    }
    readRawWeight();
    startRegister.cycleReady = nau7802::CycleReady::NotReady;
  }
}

void libopenpresso::Nau7802WeightSensorI2cControl::flushSamples(size_t count)
{
  auto startRegister = nau7802::PowerUpRegisterData{
    .voltageSelect = AVDD_SOURCE,
    .cycleStart = nau7802::CycleStart::Start,
    .analogPower = nau7802::AnalogCircuitPower::PowerUp,
    .digitalPower = nau7802::DigitalCircuitPower::PowerUp,
  };
  m_control.writeRegisters(startRegister);

  for (size_t i = 0; i < count; ++i) {
    while (startRegister.cycleReady == nau7802::CycleReady::NotReady) {
      std::this_thread::sleep_for(10ms);
      m_control.readRegisters(startRegister);
    }
    readRawWeight();
    startRegister.cycleReady = nau7802::CycleReady::NotReady;
  }
}

void Nau7802WeightSensorI2cControl::resetChip(const std::future<void>& exit)
{
  m_control.resetRegisters();
  if (exit.wait_for(10ms) == std::future_status::ready) {
    return;
  }
}

void Nau7802WeightSensorI2cControl::enableDigital(const std::future<void>& exit)
{
  m_control.writeRegisters(nau7802::PowerUpRegisterData{
    .digitalPower = nau7802::DigitalCircuitPower::PowerUp,
  });

  if (exit.wait_for(1ms) == std::future_status::ready) {
    return;
  }
}

void Nau7802WeightSensorI2cControl::enableAnalog(const std::future<void>& exit)
{
  m_control.writeRegisters(
    nau7802::PowerUpRegisterData{
      .voltageSelect = AVDD_SOURCE,
      .analogPower = nau7802::AnalogCircuitPower::PowerUp,
      .digitalPower = nau7802::DigitalCircuitPower::PowerUp,
    },
    nau7802::Control1RegisterData{
      .ldoVoltage = LDO_VOLTAGE,
      .gainSelect = PGA_MODE,
    },
    nau7802::Control2RegisterData{
      .rateSelect = CONVERSION_RATE,
    });

  m_control.writeRegisters(nau7802::AdcRegisterData{.chopperClock = nau7802::ChopperClock::TurnedOff});

  if (exit.wait_for(600ms) == std::future_status::ready) {
    return;
  }
}
