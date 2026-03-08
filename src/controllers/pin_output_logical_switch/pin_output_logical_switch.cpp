#include "pin_output_logical_switch.hpp"

#include <memory>

#include <gpio/pin_output.hpp>

libopenpresso::PinOutputLogicalSwitch::PinOutputLogicalSwitch(const std::shared_ptr<gpio::PinOutput>& output,
                                                              bool inverted)
: m_state{output->getInitState() != inverted}
, m_inverted{inverted}
, m_output{output}
{
}

void libopenpresso::PinOutputLogicalSwitch::activate()
{
  m_output->set(m_state != m_inverted);
  m_isActive = true;
}

void libopenpresso::PinOutputLogicalSwitch::deactivate()
{
  m_output->returnToInitState();
  m_isActive = false;
}

bool libopenpresso::PinOutputLogicalSwitch::isActive() const noexcept
{
  return m_isActive;
}

bool libopenpresso::PinOutputLogicalSwitch::getState() const
{
  return m_state;
}

void libopenpresso::PinOutputLogicalSwitch::setState(bool val)
{
  if (m_isActive) {
    m_output->set(val != m_inverted);
  }
  m_state = val;
}
