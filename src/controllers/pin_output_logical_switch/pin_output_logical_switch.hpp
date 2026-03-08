#ifndef CONTROLLERS_PIN_OUTPUT_LOGICAL_SWITCH_PIN_OUTPUT_LOGICAL_SWITCH_HPP
#define CONTROLLERS_PIN_OUTPUT_LOGICAL_SWITCH_PIN_OUTPUT_LOGICAL_SWITCH_HPP

#include <memory>

#include <libopenpresso/interfaces/logical_output.hpp>

namespace libopenpresso
{

namespace gpio
{
class PinOutput;
}

class PinOutputLogicalSwitch : public interfaces::LogicalOutput {
public:
  PinOutputLogicalSwitch(const std::shared_ptr<gpio::PinOutput>& output, bool inverted);
  PinOutputLogicalSwitch(const PinOutputLogicalSwitch&) = delete;
  PinOutputLogicalSwitch(PinOutputLogicalSwitch&&) = delete;
  auto operator=(const PinOutputLogicalSwitch&) = delete;
  auto operator=(PinOutputLogicalSwitch&&) = delete;
  ~PinOutputLogicalSwitch() = default;

  void activate() override;
  void deactivate() override;
  bool isActive() const noexcept override;
  bool getState() const override;
  void setState(bool val) override;

private:
  bool m_isActive = false;
  bool m_state;
  bool m_inverted;
  std::shared_ptr<gpio::PinOutput> m_output;
};

} // namespace libopenpresso

#endif // CONTROLLERS_PIN_OUTPUT_LOGICAL_SWITCH_PIN_OUTPUT_LOGICAL_SWITCH_HPP