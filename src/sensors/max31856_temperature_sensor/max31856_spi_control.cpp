#include "max31856_spi_control.hpp"

#include <cstdint>
#include <cstdlib>

#include <libopenpresso/types.hpp>

#include <max31856/max31856_config.hpp>

using namespace libopenpresso;

Max31856TemperatureSensorSpiControl::Max31856TemperatureSensorSpiControl(const unix_dev_addr_t& spiDev)
: m_control{spiDev}
{
  auto config = max31856::ConfigData{};
  config.conversionMode = max31856::ConversionMode::AutomaticConversion;
  config.noiseRejectionFilterFreq = max31856::NoiseRejectionFilterFreq::_50Hz;
  m_control.writeConfig(config);
}

Max31856TemperatureSensorSpiControl::~Max31856TemperatureSensorSpiControl()
{
  try {
    m_control.resetConfig();
  }
  catch (...) {
    std::abort();
  }
}

int32_t Max31856TemperatureSensorSpiControl::readTemperature()
{
  return m_control.readThermocoupleTemperature();
}
