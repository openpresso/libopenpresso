#include "pin_output_validator.hpp"

#include <atomic>
#include <cstdlib>
#include <memory>

#include <gpio/pin_output.hpp>
#include <watchdog/watchdog_thread.hpp>

using namespace libopenpresso::watchdog;

PinOutputValidator::PinOutputValidator(const std::shared_ptr<WatchdogThread>& watchdog,
                                       const std::shared_ptr<gpio::PinOutput>& output)
: m_watchdog{watchdog}
, m_output{output}
, m_descriptor{
    watchdog->registerValidator([this] { return !m_isFailed.load(std::memory_order_relaxed); })}
{
}

PinOutputValidator::~PinOutputValidator()
{
  try {
    m_output->returnToInitState();
    m_watchdog->unregisterValidator(m_descriptor);
  }
  catch (...) {
    std::abort();
  }
}

bool PinOutputValidator::getInitState() const noexcept
{
  return m_output->getInitState();
}

void PinOutputValidator::returnToInitState()
{
  set(getInitState());
}

bool PinOutputValidator::get() const noexcept
{
  return m_output->get();
}

void PinOutputValidator::set(bool val)
{
  try {
    m_output->set(val);
  }
  catch (...) {
    m_isFailed.store(true, std::memory_order_relaxed);
    throw;
  }
}
