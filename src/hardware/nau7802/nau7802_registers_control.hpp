#ifndef HARDWARE_NAU7802_NAU7802_REGISTERS_CONTROL_HPP
#define HARDWARE_NAU7802_NAU7802_REGISTERS_CONTROL_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <tuple>
#include <utility>

#include <libopenpresso/types.hpp>

#include <utils/fd_wrapper.hpp>

namespace libopenpresso::i2c
{
class I2cBus;
}

namespace libopenpresso::nau7802
{

template <typename... Ts>
concept SequentialRegisters = []<std::size_t... I>(std::index_sequence<I...>) {
  using Tup = std::tuple<Ts...>;
  return ((std::to_underlying(std::tuple_element_t<I, Tup>::addr) + 1 ==
           std::to_underlying(std::tuple_element_t<I + 1, Tup>::addr)) &&
          ...);
}(std::make_index_sequence<sizeof...(Ts) - 1>{});

class RegistersControl {
  static constexpr size_t WEIGHT_REGISTER_SIZE = 3;

public:
  RegistersControl(const std::shared_ptr<i2c::I2cBus>& bus, i2c_dev_addr_t dev);

  int32_t readConversionResult();
  void resetRegisters();

  bool isAnalogPowerUp();

  template <typename... T>
    requires SequentialRegisters<T...>
  void readRegisters(T&... regs)
  {
    std::array<uint8_t, sizeof...(T)> buf{};
    auto addr = std::to_underlying(std::tuple_element_t<0, std::tuple<T...>>::addr);
    readRaw(addr, buf);
    [&]<size_t... I>(std::index_sequence<I...>) {
      ((regs = T::decode(buf[I])), ...);
    }(std::make_index_sequence<sizeof...(T)>{});
  }

  template <typename... T>
    requires SequentialRegisters<T...>
  void writeRegisters(const T&... regs)
  {
    std::array<uint8_t, sizeof...(T) + 1> buf{};
    buf[0] = std::to_underlying(std::tuple_element_t<0, std::tuple<T...>>::addr);
    [&]<size_t... I>(std::index_sequence<I...>) {
      (regs.encode(buf[I + 1]), ...);
    }(std::make_index_sequence<sizeof...(T)>{});
    writeRaw(buf);
  }

  void readRaw(uint8_t startAddr, std::span<uint8_t> data);
  void writeRaw(std::span<const uint8_t> data);

private:
  const std::shared_ptr<i2c::I2cBus> m_bus;
  i2c_dev_addr_t m_dev;
  const fd_wrapper m_i2cFd;
};

} // namespace libopenpresso::nau7802

#endif // HARDWARE_NAU7802_NAU7802_REGISTERS_CONTROL_HPP