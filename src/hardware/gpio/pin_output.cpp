#include "pin_output.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>

#include <libopenpresso/exception.hpp>

#include <gpio/pin_data.hpp>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <utils/fd_wrapper.hpp>

using namespace libopenpresso::gpio;

PinOutputImpl::PinOutputImpl(const output_pin_info_t& info)
: m_initState(info.initState)
, m_val(info.initState)
{
  gpio_v2_line_request lineRequest{};
  fd_wrapper chipFd(open(info.pinAddr.chip.c_str(), O_RDONLY));
  lineRequest.num_lines = 1;
  lineRequest.offsets[0] = info.pinAddr.pin;
  lineRequest.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;

  strncpy(&lineRequest.consumer[0], info.label.c_str(), GPIO_MAX_NAME_SIZE - 1);

  int res = ioctl(chipFd.get(), GPIO_V2_GET_LINE_IOCTL, &lineRequest);
  if (res < 0) {
    throw libopenpresso::SystemError{"Failed to activate gpio output"};
  }

  m_controlDescriptor = fd_wrapper{lineRequest.fd};
  gpioSet(m_initState);
}

PinOutputImpl::~PinOutputImpl()
{
  try {
    returnToInitState();
  }
  catch (...) {
    std::abort();
  }
}

bool PinOutputImpl::get() const noexcept
{
  return m_val;
}

bool PinOutputImpl::getInitState() const noexcept
{
  return m_initState;
}

void PinOutputImpl::returnToInitState()
{
  set(m_initState);
}

void PinOutputImpl::set(bool val)
{
  if (m_val == val) {
    return;
  }

  gpioSet(val);

  m_val = val;
}

void PinOutputImpl::gpioSet(bool val)
{
  gpio_v2_line_values vals{};
  vals.mask = 1;
  vals.bits = val ? 1 : 0;
  int res = ioctl(m_controlDescriptor.get(), GPIO_V2_LINE_SET_VALUES_IOCTL, &vals);
  if (res < 0) {
    throw libopenpresso::SystemError{"Failed to set gpio output value"};
  }
}
