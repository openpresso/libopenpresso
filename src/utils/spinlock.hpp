#ifndef UTILS_SPINLOCK_HPP
#define UTILS_SPINLOCK_HPP

#include <atomic>
#include <thread>

class spinlock {
public:
  spinlock(bool locked = false) noexcept
  {
    if (locked) {
      m_flag.test_and_set();
    }
  }
  spinlock(const spinlock&) = delete;
  spinlock(spinlock&&) = delete;
  auto operator=(spinlock&&) -> spinlock& = delete;
  auto operator=(const spinlock&) -> spinlock& = delete;
  ~spinlock() = default;

  void lock() noexcept
  {
    while (m_flag.test_and_set(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }

  void unlock() noexcept
  {
    m_flag.clear(std::memory_order_release);
  }

  bool try_lock() noexcept
  {
    return !m_flag.test_and_set(std::memory_order_acquire);
  }

private:
  std::atomic_flag m_flag;
};

#endif