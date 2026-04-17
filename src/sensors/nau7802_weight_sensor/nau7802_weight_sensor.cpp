#include "nau7802_weight_sensor.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <nau7802/nau7802_registers_control.hpp>
#include <nau7802_weight_sensor/nau7802_i2c_control.hpp>
#include <utils/logger.hpp>

using namespace libopenpresso;

Nau7802WeightSensor::Nau7802WeightSensor(const std::shared_ptr<i2c::I2cBus>& bus,
                                         i2c_dev_addr_t dev,
                                         const std::shared_ptr<gpio::PinMonitor>& monitor,
                                         uint32_t scale,
                                         time_delta_t flowRateSmoothingTime)
: m_scale{static_cast<int32_t>(scale)}
, m_tareLock{true}
, m_dFiler{std::chrono::duration<float>(flowRateSmoothingTime).count(),
           1.0f / Nau7802WeightSensorI2cControl::sps()}
, m_i2cControl{bus, dev, [this, monitor] {
                 m_tareLock.unlock();
                 m_i2cControl.readRawWeight();
                 m_monitorCallback.emplace(monitor, gpio::PinEvent::RisingEdge, makeCallback());
               }}
{
}

Nau7802WeightSensor::~Nau7802WeightSensor()
{
  try {
    m_i2cControl.joinInitThread();
    m_monitorCallback.reset();
  }
  catch (...) {
    std::abort();
  }
}

void Nau7802WeightSensor::tare()
{
  std::scoped_lock lock(m_tareLock);
  m_i2cControl.resetZero();
  m_i2cControl.readRawWeight();
  m_weight.store(0, std::memory_order_relaxed);
  m_rate.store(0, std::memory_order_relaxed);
  m_dFiler.reset();
}

milligrams_t Nau7802WeightSensor::getWeight() const
{
  return m_weight.load(std::memory_order_relaxed);
}

milligrams_p_second_t Nau7802WeightSensor::getFlowRate() const
{
  return m_rate.load(std::memory_order_relaxed);
}

std::optional<time_delta_t> Nau7802WeightSensor::fixedUpdateRate() const noexcept
{
  using namespace std::chrono_literals;
  return time_delta_t{1s} / Nau7802WeightSensorI2cControl::sps();
}

callback_descriptor_t Nau7802WeightSensor::registerCallback(const callback_t& cb)
{
  if (!cb) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks.emplace(m_nextCallbackDescriptor, cb);
  return m_nextCallbackDescriptor++;
}

void Nau7802WeightSensor::unregisterCallback(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_callbacksLock);
  if (auto it = m_callbacks.find(descr); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

gpio::PinMonitor::PinEventCallback_t Nau7802WeightSensor::makeCallback()
{
  return [this](gpio::PinEvent) {
    milligrams_t weight = 0;
    milligrams_p_second_t rate = 0;

    try {
      std::unique_lock lock(m_tareLock, std::try_to_lock);
      if (lock.owns_lock()) {
        weight = (m_i2cControl.readRawWeight() * m_scale) >> WEIGHT_SCALE_BIT_OFFSET;
        rate = m_dFiler.process(weight);
        m_weight.store(weight, std::memory_order_relaxed);
        m_rate.store(rate, std::memory_order_relaxed);
      }
    }
    catch (const libopenpresso::Exception& e) {
      Logger::warn("Read result failed, reason: {}, thrown from file: {}, function: {}, line: {}",
                   e.what(),
                   e.throwLocation().file_name(),
                   e.throwLocation().function_name(),
                   e.throwLocation().line());
    }

    std::scoped_lock lock(m_callbacksLock);
    for (auto&& cb : m_callbacks) {
      std::invoke(cb.second, weight, rate);
    }
  };
}
