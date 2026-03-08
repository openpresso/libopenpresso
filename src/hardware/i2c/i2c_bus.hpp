#ifndef HARDWARE_I2C_I2C_BUS_HPP
#define HARDWARE_I2C_I2C_BUS_HPP

#include <cstdint>
#include <mutex>
#include <span>

#include <libopenpresso/types.hpp>

#include <utils/fd_wrapper.hpp>

namespace libopenpresso::i2c
{

class I2cBus {
public:
  I2cBus(const unix_dev_addr_t& bus);

  void lock();
  void unlock();

  void setAddr(i2c_dev_addr_t addr);
  void read(std::span<uint8_t> data);
  void write(std::span<const uint8_t> data);

private:
  const fd_wrapper m_i2cFd;
  i2c_dev_addr_t m_lastAddr = 0;
  std::mutex m_lock;
};

} // namespace libopenpresso::i2c

#endif // HARDWARE_I2C_I2C_BUS_HPP