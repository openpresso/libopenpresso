#include "i2c_bus.hpp"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <span>
#include <unistd.h>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

using namespace libopenpresso::i2c;

I2cBus::I2cBus(const unix_dev_addr_t& bus)
: m_i2cFd{open(bus.c_str(), O_RDWR)}
{
}

void I2cBus::lock()
{
  m_lock.lock();
}

void I2cBus::unlock()
{
  m_lock.unlock();
}

void I2cBus::setAddr(i2c_dev_addr_t addr)
{
  if (m_lastAddr == addr) {
    return;
  }

  if (ioctl(m_i2cFd.get(), I2C_SLAVE, static_cast<int>(addr)) < 0) {
    throw libopenpresso::SystemError{"Failed to set i2c device address"};
  }

  m_lastAddr = addr;
}

void I2cBus::read(std::span<uint8_t> data)
{
  if (auto res = ::read(m_i2cFd.get(), data.data(), data.size());
      res < 0 || static_cast<size_t>(res) != data.size()) {
    throw libopenpresso::SystemError{"Failed to read i2c data"};
  }
}

void I2cBus::write(std::span<const uint8_t> data)
{
  if (auto res = ::write(m_i2cFd.get(), data.data(), data.size());
      res < 0 || static_cast<size_t>(res) != data.size()) {
    throw libopenpresso::SystemError{"Failed to write i2c data"};
  }
}
