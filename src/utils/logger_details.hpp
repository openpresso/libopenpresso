#ifndef UTILS_LOGGER_DETAILS_HPP
#define UTILS_LOGGER_DETAILS_HPP

#include <source_location>

#ifdef LIBOPENPRESSO_LOGS_ENABLE

#include <concepts>
#include <memory>
#include <type_traits>

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/null_sink.h>

namespace libopenpresso::logger_details
{
using LogLevel = spdlog::level::level_enum;

template <class... Args>
struct format_with_location {
  template <std::convertible_to<spdlog::format_string_t<Args...>> T>
  consteval format_with_location(T const& fmt, std::source_location loc = std::source_location::current())
  : fmt(fmt)
  , loc(loc)
  {
  }

  spdlog::format_string_t<Args...> fmt;
  std::source_location loc;
};

template <typename... Args>
using format_string_t = format_with_location<std::type_identity_t<Args>...>;

struct Instance {
  static const std::shared_ptr<spdlog::logger>& get()
  {
    return instance();
  }

  static void set(const std::shared_ptr<spdlog::logger>& logger)
  {
    instance() = logger ? logger : spdlog::null_logger_mt(std::string{});
  }

private:
  static std::shared_ptr<spdlog::logger>& instance()
  {
    static std::shared_ptr<spdlog::logger> logger;
    return logger;
  }
};

spdlog::source_loc inline location_cast(std::source_location loc)
{
  return spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()};
}

} // namespace libopenpresso::logger_details

#ifndef LIBOPENPRESSO_MIN_LOG_LEVEL
#define LIBOPENPRESSO_MIN_LOG_LEVEL info
#endif

#else

#include <cstdint>
#include <format>

namespace libopenpresso::logger_details
{

enum class LogLevel : uint8_t {
  trace,
  debug,
  info,
  warn,
  err,
  critical,
  off,
};

#define LIBOPENPRESSO_MIN_LOG_LEVEL off

template <class... Args>
using format_string_t = std::format_string<Args...>;

struct Instance {
  static Instance* get()
  {
    static Instance logger;
    return &logger;
  }

  static void set([[maybe_unused]] auto logger)
  {
  }

  template <LogLevel, typename... Args>
  void log([[maybe_unused]] auto loc,
           [[maybe_unused]] format_string_t<Args...> fmt,
           [[maybe_unused]] Args&&... args)
  {
  }
};

auto inline location_cast(std::source_location loc)
{
  return loc;
}

} // namespace libopenpresso::logger_details

#endif

#endif // UTILS_LOGGER_DETAILS_HPP