#include "virtual_weight_sensor.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <ratio>
#include <utility>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <pump_flow_sensor/pump_flow_sensor.hpp>
#include <utils/derivative_filter.hpp>

using namespace libopenpresso;

void VirtualWeightSensor::processTareFlag(micrograms_t& weight, auto& dFilter)
{
  if (m_tareFlag.load(std::memory_order_acquire)) {
    m_weight.store(0, std::memory_order_relaxed);
    m_flowRate.store(0, std::memory_order_relaxed);
    dFilter.reset();
    weight = 0;
    m_tareFlag.store(false, std::memory_order_release);
    m_tareFlag.notify_one();
  }
}

auto VirtualWeightSensor::getFixedRateCallback(std::chrono::duration<float> dt,
                                               std::chrono::duration<float> smoothing)
{
  return [this,
          weightTotal = micrograms_t{0},
          filter = FilteredDerivative<float, true>(smoothing.count(),
                                                   dt.count())](micrograms_t weightAddition) mutable {
    processTareFlag(weightTotal, filter);

    weightTotal += weightAddition;
    milligrams_t weightResult = weightTotal / std::milli::den;

    storeAndNotify(weightResult, filter.process(weightResult));
  };
}

auto VirtualWeightSensor::getVariableRateCallback(std::chrono::duration<float> smoothing)
{
  return [this,
          weightTotal = micrograms_t{0},
          filter = FilteredDerivative<float, false>(smoothing.count()),
          prevTime = std::chrono::steady_clock::now()](micrograms_t weightAddition) mutable {
    processTareFlag(weightTotal, filter);

    weightTotal += weightAddition;
    milligrams_t weightResult = weightTotal / std::milli::den;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> dt = (now - std::exchange(prevTime, now));

    storeAndNotify(weightResult, filter.process(weightResult, dt.count()));
  };
}

VirtualWeightSensor::VirtualWeightSensor(const std::shared_ptr<PumpFlowSensor>& flow,
                                         time_delta_t flowRateSmoothingTime)
: m_prevTime{std::chrono::steady_clock::now()}
, m_fixedDt{flow->fixedUpdateRate()}
{
  if (m_fixedDt.has_value()) {
    m_flowCallback.emplace(
      flow, flow->registerCallback(getFixedRateCallback(m_fixedDt.value(), flowRateSmoothingTime)));
  }
  else {
    m_flowCallback.emplace(flow, flow->registerCallback(getVariableRateCallback(flowRateSmoothingTime)));
  }
}

void VirtualWeightSensor::tare()
{
  if (!m_tareFlag.exchange(true, std::memory_order_acq_rel)) {
    m_tareFlag.wait(true, std::memory_order_relaxed);
  }
}

milligrams_t VirtualWeightSensor::getWeight() const
{
  return m_weight.load(std::memory_order_relaxed);
}

milligrams_p_second_t VirtualWeightSensor::getFlowRate() const
{
  return m_flowRate.load(std::memory_order_relaxed);
}

std::optional<time_delta_t> VirtualWeightSensor::fixedUpdateRate() const noexcept
{
  return m_fixedDt;
}

callback_descriptor_t VirtualWeightSensor::registerCallback(const callback_t& callback)
{
  if (!callback) {
    throw libopenpresso::Exception{"Callback is invalid"};
  }

  std::scoped_lock lock(m_callbacksLock);
  m_callbacks.emplace(m_nextCallbackDescriptor, callback);
  return m_nextCallbackDescriptor++;
}

void VirtualWeightSensor::unregisterCallback(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_callbacksLock);
  if (auto it = m_callbacks.find(descr); it != m_callbacks.end()) {
    m_callbacks.erase(it);
  }
}

void VirtualWeightSensor::storeAndNotify(milligrams_t weight, milligrams_p_second_t rate)
{
  m_weight.store(weight, std::memory_order_relaxed);
  m_flowRate.store(rate, std::memory_order_relaxed);
  std::scoped_lock lock(m_callbacksLock);
  for (auto&& cb : m_callbacks) {
    std::invoke(cb.second, weight, rate);
  }
}
