#ifndef UTILS_CALLBACK_DESCRIPTOR_HANDLER_HPP
#define UTILS_CALLBACK_DESCRIPTOR_HANDLER_HPP

#include <functional>
#include <memory>
#include <utility>

#include <libopenpresso/types.hpp>

namespace libopenpresso
{

class CallbackDescriptorHandler {
  using unregister_func_t = void (*)(void* caller, callback_descriptor_t descr);

public:
  template <typename T>
  CallbackDescriptorHandler(const std::shared_ptr<T>& caller, callback_descriptor_t descr) noexcept
  : m_caller{caller}
  , m_descr{descr}
  , m_unregister{[](void* caller, callback_descriptor_t descr) {
    static_cast<T*>(caller)->unregisterCallback(descr);
  }}
  {
  }

  CallbackDescriptorHandler(CallbackDescriptorHandler&& other) noexcept
  : m_caller{std::move(other.m_caller)}
  , m_descr{other.m_descr}
  , m_unregister{std::exchange(other.m_unregister, [](auto, auto) {})}
  {
  }

  ~CallbackDescriptorHandler()
  {
    std::invoke(m_unregister, m_caller.get(), m_descr);
  }

  CallbackDescriptorHandler(const CallbackDescriptorHandler&) = delete;
  auto operator=(const CallbackDescriptorHandler&) -> CallbackDescriptorHandler& = delete;

  auto operator=(CallbackDescriptorHandler&& other) noexcept -> CallbackDescriptorHandler&
  {
    std::swap(m_caller, other.m_caller);
    std::swap(m_descr, other.m_descr);
    std::swap(m_unregister, other.m_unregister);
    return *this;
  }

private:
  std::shared_ptr<void> m_caller;
  callback_descriptor_t m_descr;
  unregister_func_t m_unregister;
};

} // namespace libopenpresso

#endif // UTILS_CALLBACK_DESCRIPTOR_HANDLER_HPP