#ifndef CORE_COMPONENTS_PINS_HPP
#define CORE_COMPONENTS_PINS_HPP

#include <list>
#include <ranges>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/config.hpp>
#include <libopenpresso/pin_info.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>

namespace libopenpresso::components_pins
{

template <class T>
  requires requires(T&& t) { component_config_t{t}; }
std::list<gpio::any_pin_info_t> getPinsInfo([[maybe_unused]] const component_label_t& label,
                                            [[maybe_unused]] const T& component)
{
  return {};
}

inline std::list<gpio::any_pin_info_t> getPinsInfo(const component_label_t& label,
                                                   const Ads1115PressureSensorConfig& component)
{
  gpio::input_pin_info_t signalPinInfo;
  signalPinInfo.pinAddr = component.signalPin;
  signalPinInfo.label = label;
  signalPinInfo.listenEvents = gpio::PinEvent::FallingEdge;
  signalPinInfo.pinPull = PinPull::PullUp;
  signalPinInfo.timeCritical = false;
  return {signalPinInfo};
}

inline std::list<gpio::any_pin_info_t> getPinsInfo(const component_label_t& label,
                                                   const Nau7802WeightSensorConfig& component)
{
  gpio::input_pin_info_t signalPinInfo;
  signalPinInfo.pinAddr = component.signalPin;
  signalPinInfo.label = label;
  signalPinInfo.listenEvents = gpio::PinEvent::RisingEdge;
  signalPinInfo.pinPull = PinPull::PullDown;
  signalPinInfo.timeCritical = false;
  return {signalPinInfo};
}

inline std::list<gpio::any_pin_info_t> getPinsInfo(const component_label_t& label,
                                                   const Max31856TemperatureSensorConfig& component)
{
  gpio::input_pin_info_t signalPinInfo;
  signalPinInfo.pinAddr = component.signalPin;
  signalPinInfo.label = label;
  signalPinInfo.listenEvents = gpio::PinEvent::FallingEdge;
  signalPinInfo.pinPull = PinPull::PullUp;
  signalPinInfo.timeCritical = false;
  return {signalPinInfo};
}

inline std::list<gpio::any_pin_info_t> getPinsInfo(const component_label_t& label,
                                                   const AcZeroCrossSensorConfig& component)
{
  gpio::input_pin_info_t signalPinInfo;
  signalPinInfo.pinAddr = component.signalPin;
  signalPinInfo.label = label;
  signalPinInfo.listenEvents = gpio::PinEvent::Both;
  signalPinInfo.pinPull = PinPull::PullDown;
  signalPinInfo.timeCritical = true;
  return {signalPinInfo};
}

inline std::list<gpio::any_pin_info_t> getPinsInfo(const component_label_t& label,
                                                   const LogicalInputPinConfig& component)
{
  gpio::input_pin_info_t signalPinInfo;
  signalPinInfo.pinAddr = component.addr;
  signalPinInfo.label = label;
  signalPinInfo.listenEvents = gpio::PinEvent::Both;
  signalPinInfo.pinPull = component.pull;
  signalPinInfo.timeCritical = false;
  return {signalPinInfo};
}

inline std::list<gpio::any_pin_info_t> getPinsInfo(const component_label_t& label,
                                                   const PulseControlledDeviceConfig& component)
{
  gpio::output_pin_info_t controlPinInfo;
  controlPinInfo.pinAddr = component.pulsePin;
  controlPinInfo.label = label;
  controlPinInfo.initState = false;
  return {controlPinInfo};
}

inline std::list<gpio::any_pin_info_t> getPinsInfo(const component_label_t& label,
                                                   const LogicalOutputPinConfig& component)
{
  gpio::output_pin_info_t controlPinInfo;
  controlPinInfo.pinAddr = component.addr;
  controlPinInfo.label = label;
  controlPinInfo.initState = component.initState != component.inverted;
  return {controlPinInfo};
}

static constexpr auto components_pin_view =
  std::views::transform([](auto&& component) {
    return std::visit([label = component.first](auto&& config) { return getPinsInfo(label, config); },
                      component.second);
  }) |
  std::views::join;

} // namespace libopenpresso::components_pins

#endif // CORE_COMPONENTS_PINS_HPP