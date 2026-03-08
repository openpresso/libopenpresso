#include "pulse_controller.hpp"

#include <functional>
#include <memory>
#include <utility>

#include <libopenpresso/exception.hpp>

#include <gpio/pin_data.hpp>
#include <gpio/pin_monitor.hpp>
#include <gpio/pin_output.hpp>

using namespace libopenpresso::gpio;

PulseController::PulseController(const std::shared_ptr<PinMonitor>& zeroCrossMonitor,
                                 const std::shared_ptr<PinOutput>& controlPin)
: m_control{controlPin}
, m_zeroCrossMonitor{zeroCrossMonitor}
{
}

void PulseController::setPulseCounter(const std::shared_ptr<PulseCounter>& counter)
{
  if (isActive()) {
    throw libopenpresso::Exception{"Cannot set pulse counter while active"};
  }

  if (m_counter) {
    throw libopenpresso::Exception{"Pulse counter is already set"};
  }

  m_counter = counter;
}

void PulseController::unsetPulseCounter()
{
  if (isActive()) {
    throw libopenpresso::Exception{"Cannot unset pulse counter while active"};
  }

  m_counter.reset();
}

void PulseController::activate(std::function<bool()> firePulseCallback)
{
  if (isActive()) {
    throw libopenpresso::Exception{"PulseController is already active"};
  }

  if (m_counter) {
    auto zeroCrossCallback =
      [control = m_control, counter = m_counter, cb = std::move(firePulseCallback), phase = true](auto) mutable {
        if (std::exchange(phase, !phase)) {
          return;
        }

        bool firePulse = std::invoke(cb);
        control->set(firePulse);
        if (firePulse) {
          counter->countPulse();
        }
      };
    m_cbDescriptor.emplace(m_zeroCrossMonitor, PinEvent::RisingEdge, std::move(zeroCrossCallback));
  }
  else {
    auto zeroCrossCallback =
      [control = m_control, cb = std::move(firePulseCallback), phase = true](auto) mutable {
        if (std::exchange(phase, !phase)) {
          return;
        }

        bool firePulse = std::invoke(cb);
        control->set(firePulse);
      };
    m_cbDescriptor.emplace(m_zeroCrossMonitor, PinEvent::RisingEdge, std::move(zeroCrossCallback));
  }
}

void PulseController::deactivate()
{
  if (!isActive()) {
    return;
  }

  m_cbDescriptor.reset();
  m_control->returnToInitState();
}

bool PulseController::isActive() const noexcept
{
  return m_cbDescriptor.has_value();
}