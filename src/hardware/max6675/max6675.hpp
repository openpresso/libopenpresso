#ifndef HARDWARE_MAX6675_MAX6675_HPP
#define HARDWARE_MAX6675_MAX6675_HPP

#include <cstdint>
#include <string>

#include <libopenpresso/types.hpp>

#include <linux/spi/spi.h>
#include <utils/fd_wrapper.hpp>

namespace libopenpresso
{

class Max6675 {
  static constexpr uint8_t SPI_MODE = SPI_MODE_0;
  static constexpr uint32_t SPI_SPEED = 500000;
  static constexpr uint8_t SPI_BITS = 8;

  static constexpr uint8_t INPUT_OPEN_BIT = 2;
  static constexpr uint8_t INPUT_OPEN_MASK = 1 << INPUT_OPEN_BIT;
  static constexpr uint8_t RESULT_OFFSET = 3;
  static constexpr uint32_t MILLIDEGREES_PER_BIT = 250;

public:
  Max6675(const unix_dev_addr_t& spiDev);
  millidegrees_t readCelsiusMillidegrees() const;
  const std::string& spiDev() const noexcept;

private:
  fd_wrapper m_devFd;
  std::string m_spiDev;
};

} // namespace libopenpresso

#endif // HARDWARE_MAX6675_MAX6675_HPP