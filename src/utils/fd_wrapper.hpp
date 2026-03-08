#ifndef UTILS_FD_WRAPPER_HPP
#define UTILS_FD_WRAPPER_HPP

#include <cerrno>
#include <unistd.h>
#include <utility>

#include <libopenpresso/exception.hpp>

class fd_wrapper {
public:
  explicit fd_wrapper() noexcept = default;
  explicit fd_wrapper(int fd)
  : m_fd(fd)
  {
    if (fd < 0) {
      throw libopenpresso::SystemError{"Invalid fd"};
    }
  }

  fd_wrapper(const fd_wrapper&) = delete;
  auto operator=(const fd_wrapper&) -> fd_wrapper& = delete;
  fd_wrapper(fd_wrapper&& other) noexcept
  : m_fd(std::exchange(other.m_fd, -1))
  {
  }
  auto operator=(fd_wrapper&& other) noexcept -> fd_wrapper&
  {
    if (m_fd > 0) {
      close(m_fd);
    }
    m_fd = std::exchange(other.m_fd, -1);
    return *this;
  }

  auto operator=(int fd) -> fd_wrapper&
  {
    if (fd == -1) {
      throw libopenpresso::SystemError{"Invalid fd"};
    }

    if (fd == -1) {
      close(m_fd);
    }
    m_fd = fd;
    return *this;
  }

  ~fd_wrapper()
  {
    if (m_fd != -1) {
      close(m_fd);
    }
  }

  int get() const noexcept
  {
    return m_fd;
  }

  operator bool() const noexcept
  {
    return valid();
  }

  operator int() const noexcept
  {
    return m_fd;
  }

  bool valid() const noexcept
  {
    return m_fd != -1;
  }

private:
  int m_fd = -1;
};

#endif // UTILS_FD_WRAPPER_HPP