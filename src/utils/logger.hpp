#ifndef UTILS_LOGGER_HPP
#define UTILS_LOGGER_HPP

#include "logger_details.hpp"

#include <utility>

namespace libopenpresso
{

using LogLevel = logger_details::LogLevel;

class Logger {
public:
  Logger() = delete;

  template <typename T>
  static void setLogger(T&& logger)
  {
    logger_details::Instance::set(std::forward<T>(logger));
  }

  template <LogLevel level, typename... Args>
    requires(level != LogLevel::off)
  static void log(logger_details::format_string_t<Args...> fmt, Args&&... args)
  {
    if constexpr (level >= LogLevel::LIBOPENPRESSO_MIN_LOG_LEVEL) {
      auto&& logger = logger_details::Instance::get();
      auto location = logger_details::location_cast(std::move(fmt.loc));
      logger->log(std::move(location), level, fmt.fmt, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  static void trace(logger_details::format_string_t<Args...> fmt, Args&&... args)
  {
    log<LogLevel::trace>(std::move(fmt), std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void debug(logger_details::format_string_t<Args...> fmt, Args&&... args)
  {
    log<LogLevel::debug>(std::move(fmt), std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void info(logger_details::format_string_t<Args...> fmt, Args&&... args)
  {
    log<LogLevel::info>(std::move(fmt), std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void warn(logger_details::format_string_t<Args...> fmt, Args&&... args)
  {
    log<LogLevel::warn>(std::move(fmt), std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void err(logger_details::format_string_t<Args...> fmt, Args&&... args)
  {
    log<LogLevel::err>(std::move(fmt), std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void critical(logger_details::format_string_t<Args...> fmt, Args&&... args)
  {
    log<LogLevel::critical>(std::move(fmt), std::forward<Args>(args)...);
  }
};

} // namespace libopenpresso

#endif // UTILS_LOGGER_HPP