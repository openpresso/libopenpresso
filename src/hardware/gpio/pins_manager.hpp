#ifndef HARDWARE_GPIO_PINS_MANAGER_HPP
#define HARDWARE_GPIO_PINS_MANAGER_HPP

#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include <libopenpresso/pin_info.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <utils/input_range_of.hpp>

namespace libopenpresso::gpio
{
class MultiPinMonitor;
class PinMonitor;
class PinOutput;

class PinsManager {
public:
  PinsManager(const std::list<any_pin_info_t>& pins);
  pin_id_t getPinId(const pin_addr_t& addr) const;
  std::shared_ptr<gpio::PinMonitor> findPinMonitor(const pin_addr_t& addr) const;
  std::shared_ptr<gpio::PinOutput> findPinOutput(const pin_addr_t& addr) const;

private:
  template <input_range_of<pin_addr_t> Range>
  static std::unordered_map<unix_dev_addr_t, uint16_t> makeChipIndexes(const Range& pins);
  template <input_range_of<std::string> Range>
  static void validateLabelsLength(const Range& labels);
  template <input_range_of<std::string> Range>
  static void validateLabelsUniquness(const Range& labels);
  template <input_range_of<pin_addr_t> Range>
  void validateAddressesUniquness(const Range& addresses) const;

private:
  std::unordered_map<unix_dev_addr_t, uint16_t> m_chipIndexes;
  std::shared_ptr<MultiPinMonitor> m_pinsMonitor;
  std::unordered_map<pin_id_t, std::shared_ptr<PinMonitor>> m_timeSensetivePinMonitors;
  std::unordered_map<pin_id_t, std::shared_ptr<PinOutput>> m_pinOutputs;
};

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_PINS_MANAGER_HPP