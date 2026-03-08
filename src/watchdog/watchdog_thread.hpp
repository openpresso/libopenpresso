#ifndef WATCHDOG_WATCHDOG_THREAD_HPP
#define WATCHDOG_WATCHDOG_THREAD_HPP

#include <cstddef>
#include <functional>
#include <future>
#include <thread>
#include <unordered_map>

#include <libopenpresso/types.hpp>

#include <utils/fd_wrapper.hpp>
#include <utils/spinlock.hpp>

namespace libopenpresso::watchdog
{

class WatchdogThread {
public:
  using validator_t = std::function<bool(void)>;
  WatchdogThread(const unix_dev_addr_t& addr, time_delta_t timeout);
  WatchdogThread(const WatchdogThread&) = delete;
  WatchdogThread(WatchdogThread&&) = delete;
  auto operator=(const WatchdogThread&) = delete;
  auto operator=(WatchdogThread&&) = delete;
  ~WatchdogThread();

  time_delta_t validationInterval() const;
  callback_descriptor_t registerValidator(const validator_t& validator);
  void unregisterValidator(callback_descriptor_t descr);

private:
  void worker(std::future<void> exit, const fd_wrapper& watchdogFd);

private:
  time_delta_t m_validationInterval = {};
  size_t m_nextValidatorDescriptor = 0;
  spinlock m_validatorsLock;
  std::unordered_map<callback_descriptor_t, validator_t> m_validators;
  std::promise<void> m_exit;
  std::thread m_worker;
};

} // namespace libopenpresso::watchdog

#endif // WATCHDOG_WATCHDOG_THREAD_HPP