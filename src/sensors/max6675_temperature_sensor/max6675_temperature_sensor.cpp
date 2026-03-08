#include "max6675_temperature_sensor.hpp"

#include <atomic>
#include <cstdlib>
#include <exception>
#include <functional>
#include <future>
#include <mutex>
#include <optional>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <utils/logger.hpp>

using namespace libopenpresso;

Max6675TemperatureSensor::Max6675TemperatureSensor(const unix_dev_addr_t& spiDev)
: m_adc{spiDev}
, m_monitorThread{&Max6675TemperatureSensor::worker, this, m_exit.get_future()}
{
}

libopenpresso::Max6675TemperatureSensor::~Max6675TemperatureSensor()
{
  try {
    m_exit.set_value();
    m_monitorThread.join();
  }
  catch (...) {
    std::abort();
  }
}

millidegrees_t Max6675TemperatureSensor::getTemperature() const
{
  return m_result.load(std::memory_order_relaxed);
}

std::optional<time_delta_t> libopenpresso::Max6675TemperatureSensor::fixedUpdateRate() const noexcept
{
  return SENSOR_UPDATE_TIMEOUT_MILLIS;
}

callback_descriptor_t Max6675TemperatureSensor::registerCallback(const callback_t& callback)
{
  if (!callback) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks.emplace(m_nextCallbackDescriptor, callback);
  return m_nextCallbackDescriptor++;
}

void Max6675TemperatureSensor::unregisterCallback(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_callbacksLock);
  if (auto it = m_callbacks.find(descr); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

void Max6675TemperatureSensor::worker(std::future<void> exitRequest)
{
  bool exit = false;

  while (!exit) {
    millidegrees_t result = 0;
    try {
      result = m_adc.readCelsiusMillidegrees();
      if (result == 0) {
        Logger::warn("Read invalid zero temperature from Max6675 sensor");
      }
    }
    catch (const libopenpresso::Exception& e) {
      Logger::warn("Read result failed, reason: {}, thrown from file: {}, function: {}, line: {}",
                   e.what(),
                   e.throwLocation().file_name(),
                   e.throwLocation().function_name(),
                   e.throwLocation().line());
    }

    m_result.store(result, std::memory_order_relaxed);

    try {
      std::scoped_lock lock(m_callbacksLock);
      for (auto&& cb : m_callbacks) {
        std::invoke(cb.second, result);
      }
    }
    catch (const libopenpresso::Exception& e) {
      Logger::err("Sensor callback exception caught, reason: {}, thrown from file: {}, function: "
                  "{}, line: {}",
                  e.what(),
                  e.throwLocation().file_name(),
                  e.throwLocation().function_name(),
                  e.throwLocation().line());
    }
    catch (const std::exception& e) {
      Logger::err("Epoll event handler exception caught, what: {}", e.what());
    }
    exit = exitRequest.wait_for(SENSOR_UPDATE_TIMEOUT_MILLIS) != std::future_status::timeout;
  }
}
