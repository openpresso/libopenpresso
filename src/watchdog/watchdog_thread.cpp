#include "watchdog_thread.hpp"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <fcntl.h>
#include <future>
#include <mutex>
#include <ranges>
#include <system_error>
#include <unistd.h>
#include <utility>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/types.hpp>

#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <utils/fd_wrapper.hpp>
#include <utils/logger.hpp>

using namespace libopenpresso::watchdog;

WatchdogThread::WatchdogThread(const unix_dev_addr_t& addr, time_delta_t timeout)
{
  fd_wrapper watchdogFd{open(addr.c_str(), O_RDWR)};
  int seconds = std::max<int>(std::chrono::duration_cast<std::chrono::seconds>(timeout).count(), 1);
  if (ioctl(watchdogFd.get(), WDIOC_SETTIMEOUT, &seconds) < 0) {
    throw libopenpresso::SystemError{"Failed to set watchdog timeout"};
  }

  time_delta_t actualTimeout = std::chrono::seconds{seconds};
  if (actualTimeout > timeout) {
    throw libopenpresso::Exception{
      "Cannot init watchdog with given timeout {}s, rounded by driver to {}s", timeout, actualTimeout};
  }

  m_validationInterval = actualTimeout / 2;

  m_worker = std::thread{&WatchdogThread::worker, this, m_exit.get_future(), std::move(watchdogFd)};
}

WatchdogThread::~WatchdogThread()
{
  m_exit.set_value();
  m_worker.join();
}

libopenpresso::callback_descriptor_t WatchdogThread::registerValidator(const validator_t& validator)
{
  if (!validator) {
    throw libopenpresso::Exception{"Validator is invalid"};
  }

  std::scoped_lock lock(m_validatorsLock);
  m_validators.emplace(m_nextValidatorDescriptor, validator);
  return m_nextValidatorDescriptor++;
}

std::chrono::steady_clock::duration WatchdogThread::validationInterval() const
{
  return m_validationInterval;
}

void WatchdogThread::unregisterValidator(callback_descriptor_t descr)
{
  std::scoped_lock lock(m_validatorsLock);
  if (auto it = m_validators.find(descr); it != m_validators.end()) {
    m_validators.erase(it);
  }
}

void WatchdogThread::worker(std::future<void> exit, const fd_wrapper& watchdogFd)
{
  do {
    try {
      std::unique_lock lock(m_validatorsLock);
      bool allIsValid = std::ranges::all_of(m_validators | std::views::values, &validator_t::operator());
      lock.unlock();
      if (!allIsValid) {
        Logger::critical("Watchdog validator failed");
        std::abort();
      }
    }
    catch (const libopenpresso::Exception& e) {
      Logger::critical("Watchdog validator exception caught, what: {}, thrown from file: {}, "
                       "function: {}, line: {}",
                       e.what(),
                       e.throwLocation().file_name(),
                       e.throwLocation().function_name(),
                       e.throwLocation().line());
      std::abort();
    }
    catch (const std::exception& e) {
      Logger::critical("Watchdog validator exception caught, what: {}", e.what());
      std::abort();
    }

    if (int res = ioctl(watchdogFd.get(), WDIOC_KEEPALIVE, 0); res < 0) {
      std::error_code err = {errno, std::system_category()};
      Logger::critical("Failed to kick watchdog, code: {}, message", err.value(), err.message());
      std::abort();
    }
  } while (exit.wait_for(m_validationInterval) == std::future_status::timeout);

  std::unique_lock lock(m_validatorsLock);
  if (!m_validators.empty()) {
    Logger::critical("Watchdog exit was requested with {} active validators", m_validators.size());
    std::abort();
  }

  static constexpr char MAGIC_EXIT = 'V';
  if (write(watchdogFd.get(), &MAGIC_EXIT, sizeof(MAGIC_EXIT)) != sizeof(MAGIC_EXIT)) {
    std::error_code err = {errno, std::system_category()};
    Logger::critical("Failed to write magic exit symbol, code: {}, message", err.value(), err.message());
    std::abort();
  }
}
