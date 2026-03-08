#ifndef HARDWARE_GPIO_PIN_OUTPUT_HPP
#define HARDWARE_GPIO_PIN_OUTPUT_HPP

#include <gpio/pin_data.hpp>
#include <utils/fd_wrapper.hpp>

namespace libopenpresso::gpio
{

class PinOutput {
public:
  virtual bool getInitState() const noexcept = 0;
  virtual void returnToInitState() = 0;
  virtual bool get() const noexcept = 0;
  virtual void set(bool val) = 0;
  virtual ~PinOutput() = default;
};

class PinOutputImpl : public PinOutput {
public:
  PinOutputImpl(const output_pin_info_t& info);
  PinOutputImpl(const PinOutputImpl&) = delete;
  PinOutputImpl(PinOutputImpl&&) = delete;
  auto operator=(const PinOutputImpl&) = delete;
  auto operator=(PinOutputImpl&&) = delete;
  ~PinOutputImpl();

  bool getInitState() const noexcept override;
  void returnToInitState() override;
  bool get() const noexcept override;
  void set(bool val) override;

private:
  void gpioSet(bool val);

private:
  const bool m_initState;
  bool m_val;
  fd_wrapper m_controlDescriptor;
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_PIN_OUTPUT_HPP