#include "max31856_temperature_sensor.hpp"

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <ratio>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <gpio/pin_monitor.hpp>
#include <max31856/max31856_config.hpp>
#include <max31856_temperature_sensor/max31856_spi_control.hpp>
#include <utils/logger.hpp>

using namespace libopenpresso;

Max31856TemperatureSensor::Max31856TemperatureSensor(const unix_dev_addr_t& spiDev,
                                                     const std::shared_ptr<gpio::PinMonitor>& monitor)
: Max31856TemperatureSensorSpiControl{spiDev}
, m_millidegrees{static_cast<int>(std::milli::den * readTemperature()) >> max31856::THERMOCOUPLE_DEGREE_BITS_SHIFT}
, m_callback{monitor, gpio::PinEvent::FallingEdge, makeCallback()}
{
}

millidegrees_t Max31856TemperatureSensor::getTemperature() const
{
  return m_millidegrees.load(std::memory_order_relaxed);
}

std::optional<time_delta_t> libopenpresso::Max31856TemperatureSensor::fixedUpdateRate() const noexcept
{
  using namespace std::chrono_literals;
  return 98ms;
}

callback_descriptor_t Max31856TemperatureSensor::registerCallback(const callback_t& callback)
{
  if (!callback) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks.insert({m_nextCallbackDescriptor, callback});
  return m_nextCallbackDescriptor++;
}

void Max31856TemperatureSensor::unregisterCallback(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_callbacksLock);

  if (auto it = m_callbacks.find(descr); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

gpio::PinMonitor::PinEventCallback_t Max31856TemperatureSensor::makeCallback()
{
  return [this](gpio::PinEvent) {
    int32_t rawVal = 0;
    try {
      rawVal = readTemperature();
    }
    catch (const libopenpresso::Exception& e) {
      Logger::warn("Read result failed, reason: {}, thrown from file: {}, function: {}, line: {}",
                   e.what(),
                   e.throwLocation().file_name(),
                   e.throwLocation().function_name(),
                   e.throwLocation().line());
      // we should send 0 to callbacks because we are fixed update rate sensor
    }

    millidegrees_t millidegrees = (rawVal * std::milli::den) >> max31856::THERMOCOUPLE_DEGREE_BITS_SHIFT;

    m_millidegrees.store(millidegrees, std::memory_order_relaxed);
    std::scoped_lock lock(m_callbacksLock);
    for (auto&& cb : m_callbacks) {
      std::invoke(cb.second, millidegrees);
    }
  };
}
