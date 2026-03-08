#ifndef LIBOPENPRESSO_EXCEPTION_HPP
#define LIBOPENPRESSO_EXCEPTION_HPP

#include <cerrno>
#include <concepts>
#include <format>
#include <source_location>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>

namespace libopenpresso
{

/// @cond
namespace details
{

template <class... Args>
struct format_with_location {
  template <std::convertible_to<std::format_string<Args...>> T>
  consteval format_with_location(T const& fmt, std::source_location loc = std::source_location::current())
  : fmt(fmt)
  , loc(loc)
  {
  }

  std::format_string<Args...> fmt;
  std::source_location loc;
};

template <class... Args>
struct format_location_error {
  template <std::convertible_to<std::format_string<Args...>> T>
  consteval format_location_error(T const& fmt,
                                  std::source_location loc = std::source_location::current(),
                                  std::error_code err = {errno, std::generic_category()})
  : fmt(fmt)
  , loc(loc)
  , err(err)
  {
  }

  std::format_string<Args...> fmt;
  std::source_location loc;
  std::error_code err;
};

} // namespace details
/// @endcond

/**
 * @class Exception
 * @brief General-purpose exception for libopenpresso errors.
 *
 * Extends std::runtime_error with automatic source location capture.
 * Accepts std::format-style message templates directly in its constructor,
 * so there is no need to call std::format explicitly.
 *
 * @code
 * throw libopenpresso::Exception{"Component {} not found", label};
 * @endcode
 */

/**
 * @fn template <typename... Args> Exception::Exception(const format_string_t<Args...>& fmt,
 * Args&&... args)
 * @brief Construct with a format string and arguments.
 *
 * Automatically captures throw location via std::source_location.
 *
 * @tparam Args Format argument types.
 * @param fmt Format string (compile-time checked via std::format_string).
 * @param args Arguments to format into the message.
 */

/**
 * @fn Exception::Exception(const std::string& message, std::source_location location)
 * @brief Construct with a plain string message.
 *
 * @param message Error message string.
 * @param location Source location of the throw site (auto-captured).
 */

/**
 * @fn const std::source_location& Exception::throwLocation() const noexcept
 * @brief Get the source location where the exception was thrown.
 *
 * @return Reference to the captured std::source_location.
 */

class Exception : public std::runtime_error {
  template <typename... Args>
  using format_string_t = details::format_with_location<std::type_identity_t<Args>...>;

public:
  template <typename... Args>
  explicit Exception(const format_string_t<Args...>& fmt, Args&&... args)
  : std::runtime_error{std::format(fmt.fmt, std::forward<Args>(args)...)}
  , m_location{fmt.loc}
  {
  }

  explicit Exception(const std::string& message,
                     std::source_location location = std::source_location::current())
  : std::runtime_error{message}
  , m_location{location}
  {
  }

  const std::source_location& throwLocation() const noexcept
  {
    return m_location;
  }

private:
  std::source_location m_location;
};

/**
 * @class SystemError
 * @brief Exception for OS/system call failures.
 *
 * Extends Exception with an std::error_code. Automatically captures errno at
 * the throw site and appends the system error message to the formatted string.
 *
 * @code
 * if (ioctl(fd, cmd, &arg) < 0) {
 *   throw libopenpresso::SystemError{"Failed to configure SPI on {}", devPath};
 * }
 * @endcode
 */

/**
 * @fn template <typename... Args> SystemError::SystemError(const format_string_t<Args...>& fmt,
 * Args&&... args)
 * @brief Construct with a format string and arguments.
 *
 * Automatically captures errno and std::source_location at the throw site.
 * The system error description is appended to the formatted message.
 *
 * @tparam Args Format argument types.
 * @param fmt Format string (compile-time checked).
 * @param args Arguments to format into the message.
 */

/**
 * @fn SystemError::SystemError(const std::string& message, std::source_location loc,
 * std::error_code err)
 * @brief Construct with a plain string, source location and error code.
 *
 * @param message Error message string.
 * @param loc Source location of the throw site (auto-captured).
 * @param err System error code (defaults to current errno).
 */

/**
 * @fn const std::error_code& SystemError::code() const noexcept
 * @brief Get the captured system error code.
 *
 * @return Reference to the std::error_code captured at the throw site.
 */

class SystemError : public Exception {
  template <typename... Args>
  using format_string_t = details::format_location_error<std::type_identity_t<Args>...>;

public:
  template <typename... Args>
  explicit SystemError(const format_string_t<Args...>& fmt, Args&&... args)
  : Exception{std::format(fmt.fmt, std::forward<Args>(args)...) + ": " + fmt.err.message(), fmt.loc}
  , m_error{fmt.err}
  {
  }

  explicit SystemError(const std::string& message,
                       std::source_location loc = std::source_location::current(),
                       std::error_code err = {errno, std::generic_category()})
  : Exception{message + ": " + err.message(), loc}
  , m_error{err}
  {
  }

  const std::error_code& code() const noexcept
  {
    return m_error;
  }

private:
  std::error_code m_error;
};

} // namespace libopenpresso

#endif // LIBOPENPRESSO_EXCEPTION_HPP