#ifndef HARDWARE_I2C_I2C_BUS_FABRIC_HPP
#define HARDWARE_I2C_I2C_BUS_FABRIC_HPP

#include <memory>
#include <unordered_map>

#include <libopenpresso/types.hpp>

namespace libopenpresso::i2c
{
class I2cBus;

class I2cBusFabric {
public:
  std::shared_ptr<I2cBus> getI2cBus(const unix_dev_addr_t& label);
  virtual ~I2cBusFabric() = default;

private:
  std::unordered_map<unix_dev_addr_t, std::shared_ptr<i2c::I2cBus>> m_buses;
};
} // namespace libopenpresso::i2c

#endif // HARDWARE_I2C_I2C_BUS_FABRIC_HPP