#ifndef WATCHDOG_PIN_OUTPUT_VALIDATOR_HPP
#define WATCHDOG_PIN_OUTPUT_VALIDATOR_HPP

#include <atomic>
#include <memory>

#include <libopenpresso/types.hpp>

#include <gpio/pin_output.hpp>

namespace libopenpresso::watchdog
{
class WatchdogThread;

class PinOutputValidator : public gpio::PinOutput {
public:
  PinOutputValidator(const std::shared_ptr<WatchdogThread>& watchdog,
                     const std::shared_ptr<gpio::PinOutput>& output);
  PinOutputValidator(const PinOutputValidator&) = delete;
  PinOutputValidator(PinOutputValidator&&) = delete;
  auto operator=(const PinOutputValidator&) = delete;
  auto operator=(PinOutputValidator&&) = delete;
  ~PinOutputValidator();

  bool getInitState() const noexcept override;
  void returnToInitState() override;
  bool get() const noexcept override;
  void set(bool val) override;

private:
  std::shared_ptr<WatchdogThread> m_watchdog;
  std::shared_ptr<gpio::PinOutput> m_output;
  std::atomic<bool> m_isFailed = false;
  callback_descriptor_t m_descriptor;
};

} // namespace libopenpresso::watchdog

#endif // WATCHDOG_PIN_OUTPUT_VALIDATOR_HPP