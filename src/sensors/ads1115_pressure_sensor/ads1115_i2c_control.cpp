#include "ads1115_i2c_control.hpp"

#include <cstdint>
#include <cstdlib>
#include <memory>

#include <libopenpresso/types.hpp>

#include <ads1115/ads1115_config.hpp>
#include <ads1115/ads1115_registers_control.hpp>

using namespace libopenpresso;

libopenpresso::Ads1115PressureSensorI2cControl::Ads1115PressureSensorI2cControl(
  const std::shared_ptr<i2c::I2cBus>& bus, i2c_dev_addr_t dev)
: m_control{bus, dev}
{
  m_control.setThresholdsForConversionReadyMode();
  auto config = ads1115::ConfigRegisterData{};
  config.operatingMode = ads1115::OperatingMode::Continuous;
  config.inputMultiplexer = ads1115::MultiplexerConfiguration::AIN0_GND;
  config.programmableGainAmplifier = PGA_MODE;
  config.dataRate = CONVERSION_RATE;
  config.comparatorQueue = ads1115::ComparatorQueueMode::AssertAfterOne;
  m_control.writeConfig(config);
}

libopenpresso::Ads1115PressureSensorI2cControl::~Ads1115PressureSensorI2cControl()
{
  try {
    m_control.resetConfig();
    m_control.resetThresholds();
  }
  catch (...) {
    std::abort();
  }
}

int16_t libopenpresso::Ads1115PressureSensorI2cControl::readRawPressure()
{
  return m_control.readConversionResult();
}
