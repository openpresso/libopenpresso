#include "i2c_bus_fabric.hpp"

#include "i2c_bus.hpp"

#include <memory>

#include <libopenpresso/types.hpp>

using namespace libopenpresso::i2c;

std::shared_ptr<I2cBus> I2cBusFabric::getI2cBus(const unix_dev_addr_t& label)
{
  auto it = m_buses.find(label);
  if (it != m_buses.end()) {
    return it->second;
  }

  return m_buses.emplace(label, std::make_shared<i2c::I2cBus>(label)).first->second;
}