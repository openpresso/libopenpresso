#include "max6675.hpp"

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <string>
#include <unistd.h>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

libopenpresso::Max6675::Max6675(const unix_dev_addr_t& spiDev)
: m_devFd(open(spiDev.c_str(), O_RDWR))
, m_spiDev(spiDev)
{
  if (ioctl(m_devFd.get(), SPI_IOC_WR_MODE, &SPI_MODE) < 0) {
    throw libopenpresso::SystemError{"Failed to set spi device mode"};
  }
  if (ioctl(m_devFd.get(), SPI_IOC_WR_BITS_PER_WORD, &SPI_BITS) < 0) {
    throw libopenpresso::SystemError{"Failed to set spi device word size"};
  }
  if (ioctl(m_devFd.get(), SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED) < 0) {
    throw libopenpresso::SystemError{"Failed to set spi device data rate"};
  }
}

libopenpresso::millidegrees_t libopenpresso::Max6675::readCelsiusMillidegrees() const
{
  std::array<uint8_t, 2> readBuf{};

  if (auto res = read(m_devFd.get(), readBuf.data(), readBuf.size());
      res < 0 || static_cast<size_t>(res) != readBuf.size()) {
    throw libopenpresso::SystemError{"Failed to read spi data"};
  }

  if (static_cast<bool>(readBuf[1] & INPUT_OPEN_MASK)) {
    throw libopenpresso::Exception{"Thermocouple on {} isn't connected", m_spiDev};
  }

  uint32_t rawVal = ((static_cast<uint32_t>(readBuf[0]) << 8) | readBuf[1]) >> RESULT_OFFSET;
  return rawVal * MILLIDEGREES_PER_BIT;
}

const std::string& libopenpresso::Max6675::spiDev() const noexcept
{
  return m_spiDev;
}
