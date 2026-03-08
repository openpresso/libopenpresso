#include "ads1115_pressure_sensor.hpp"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <ads1115/ads1115_config.hpp>
#include <ads1115/ads1115_registers_control.hpp>
#include <ads1115_pressure_sensor/ads1115_i2c_control.hpp>
#include <gpio/pin_data.hpp>
#include <gpio/pin_monitor.hpp>
#include <utils/logger.hpp>

using namespace libopenpresso;

Ads1115PressureSensor::Ads1115PressureSensor(const std::shared_ptr<i2c::I2cBus>& bus,
                                             i2c_dev_addr_t dev,
                                             const std::shared_ptr<gpio::PinMonitor>& monitor)
: Ads1115PressureSensorI2cControl{bus, dev}
, m_monitorCallback{monitor, gpio::PinEvent::FallingEdge, makeCallback()}
{
}

millibars_t Ads1115PressureSensor::getPressure() const
{
  return m_millibars.load(std::memory_order_relaxed);
}

std::optional<time_delta_t> libopenpresso::Ads1115PressureSensor::fixedUpdateRate() const noexcept
{
  using namespace std::chrono_literals;

  static constexpr time_delta_t second = 1s;

  // NOLINTBEGIN(readability-magic-numbers)
  switch (CONVERSION_RATE) {
  case ads1115::DataRate::SPS8:
    return second / 8;
  case ads1115::DataRate::SPS16:
    return second / 16;
  case ads1115::DataRate::SPS32:
    return second / 32;
  case ads1115::DataRate::SPS64:
    return second / 64;
  case ads1115::DataRate::SPS128:
    return second / 128;
  case ads1115::DataRate::SPS250:
    return second / 250;
  case ads1115::DataRate::SPS475:
    return second / 475;
  case ads1115::DataRate::SPS860:
    return second / 860;
  }
  return {};
  // NOLINTEND(readability-magic-numbers)
}

callback_descriptor_t Ads1115PressureSensor::registerCallback(const callback_t& callback)
{
  if (!callback) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks.emplace(m_nextCallbackDescriptor, callback);
  return m_nextCallbackDescriptor++;
}

void Ads1115PressureSensor::unregisterCallback(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_callbacksLock);
  if (auto it = m_callbacks.find(descr); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

gpio::PinMonitor::PinEventCallback_t Ads1115PressureSensor::makeCallback()
{
  static constexpr float millibarsPerVolt = 1000.0f * (TRANSDUCER_MAX_BARS - TRANSDUCER_MIN_BARS) /
                                            (TRANSDUCER_MAX_VOLTS - TRANSDUCER_MIN_VOLTS);
  static constexpr float millibarsPerBit = millibarsPerVolt * ads1115::voltsPerBit(PGA_MODE);
  static constexpr auto MIN_RAW_VAL =
    static_cast<int16_t>(TRANSDUCER_MIN_VOLTS / ads1115::voltsPerBit(PGA_MODE));
  static constexpr auto MAX_RAW_VAL =
    static_cast<int16_t>(TRANSDUCER_MAX_VOLTS / ads1115::voltsPerBit(PGA_MODE));
  static constexpr uint32_t millibarsPerBitDenBitOffset = 10;
  static constexpr uint32_t millibarsPerBitDen = 1 << millibarsPerBitDenBitOffset;
  static constexpr uint32_t millibarsPerBitNum = millibarsPerBit * millibarsPerBitDen;

  return [this](gpio::PinEvent) {
    int16_t rawVal = 0;
    try {
      rawVal = readRawPressure();
    }
    catch (const libopenpresso::Exception& e) {
      Logger::warn("Read result failed, reason: {}, thrown from file: {}, function: {}, line: {}",
                   e.what(),
                   e.throwLocation().file_name(),
                   e.throwLocation().function_name(),
                   e.throwLocation().line());
      // we should send 0 to callbacks because we are fixed update rate sensor
    }

    rawVal = std::clamp(rawVal, MIN_RAW_VAL, MAX_RAW_VAL) - MIN_RAW_VAL;
    uint32_t millibars = (static_cast<uint32_t>(rawVal) * millibarsPerBitNum) >> millibarsPerBitDenBitOffset;
    m_millibars.store(millibars, std::memory_order_release);
    std::scoped_lock lock(m_callbacksLock);
    for (auto&& cb : m_callbacks) {
      std::invoke(cb.second, millibars);
    }
  };
}
