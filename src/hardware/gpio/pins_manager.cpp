#include "pins_manager.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <list>
#include <memory>
#include <numeric>
#include <ranges>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/pin_info.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/multi_pin_monitor.hpp>
#include <gpio/pin_data.hpp>
#include <gpio/pin_output.hpp>
#include <gpio/pins_transforms.hpp>
#include <gpio/time_sensetive_pin_mointor.hpp>
#include <linux/gpio.h>
#include <utils/input_range_of.hpp>

using namespace libopenpresso::gpio;

PinsManager::PinsManager(const std::list<any_pin_info_t>& pins)
: m_chipIndexes{makeChipIndexes(pins | gpio::pin_address_view)}
{
  auto addresses = pins | gpio::pin_address_view;
  auto labels = pins | gpio::pin_label_view;
  validateLabelsLength(labels);
  validateLabelsUniquness(labels);
  validateAddressesUniquness(addresses);

  auto pair_with_pin_id = [this](auto&& pin) { return std::make_pair(getPinId(pin.pinAddr), pin); };
  static constexpr auto make_pin_output = [](auto&& pin) {
    return std::make_pair(pin.first, std::make_shared<gpio::PinOutputImpl>(pin.second));
  };
  static constexpr auto make_pin_monitor = [](auto&& pin) {
    return std::make_pair(pin.first, std::make_shared<gpio::TimeSensetivePinMonitor>(pin.second));
  };

  auto nonTimeSensetiveInputPins = std::ranges::to<gpio::MultiPinMonitor::pins_map_t>(
    pins | gpio::non_time_sensetive_input_pin_filter | std::views::transform(pair_with_pin_id));

  m_pinsMonitor = std::make_shared<gpio::MultiPinMonitor>(nonTimeSensetiveInputPins);

  m_timeSensetivePinMonitors =
    std::ranges::to<std::unordered_map<pin_id_t, std::shared_ptr<gpio::PinMonitor>>>(
      pins | gpio::time_sensetive_input_pin_filter | std::views::transform(pair_with_pin_id) |
      std::views::transform(make_pin_monitor));

  m_pinOutputs = std::ranges::to<std::unordered_map<pin_id_t, std::shared_ptr<gpio::PinOutput>>>(
    pins | gpio::output_pin_filter | std::views::transform(pair_with_pin_id) |
    std::views::transform(make_pin_output));
}

pin_id_t PinsManager::getPinId(const pin_addr_t& addr) const
{
  auto it = m_chipIndexes.find(addr.chip);
  if (it == m_chipIndexes.end()) {
    throw libopenpresso::Exception{"Unregistered gpio chip {}", addr.chip.c_str()};
  }
  return (it->second << (sizeof(pin_number_t) * 8)) + addr.pin;
}

std::shared_ptr<PinMonitor> PinsManager::findPinMonitor(const pin_addr_t& addr) const
{
  auto pinId = getPinId(addr);
  std::shared_ptr<gpio::PinMonitor> pinMonitor;
  if (auto it = m_timeSensetivePinMonitors.find(pinId); it != m_timeSensetivePinMonitors.end()) {
    pinMonitor = it->second;
  }
  else {
    pinMonitor = m_pinsMonitor->getPinMonitor(pinId);
  }

  if (!pinMonitor) {
    throw libopenpresso::Exception{
      "Pin {}:{} isn't registered for monitoring", addr.chip.string(), addr.pin};
  }

  return pinMonitor;
}

std::shared_ptr<PinOutput> PinsManager::findPinOutput(const pin_addr_t& addr) const
{
  auto it = m_pinOutputs.find(getPinId(addr));
  if (it == m_pinOutputs.end()) {
    throw libopenpresso::Exception{"Pin {}:{} isn't registered for output", addr.chip.string(), addr.pin};
  }
  return it->second;
}

template <input_range_of<libopenpresso::pin_addr_t> Range>
std::unordered_map<libopenpresso::unix_dev_addr_t, uint16_t> PinsManager::makeChipIndexes(const Range& pins)
{
  auto unique_chips = std::ranges::to<std::unordered_set<unix_dev_addr_t>>(
    pins | std::views::transform([](auto&& addr) { return addr.chip; }));

  return std::ranges::to<std::unordered_map<unix_dev_addr_t, uint16_t>>(
    unique_chips | std::views::enumerate | std::views::transform([](auto&& chip) {
      return std::make_pair(std::get<1>(chip), std::get<0>(chip));
    }));
}

template <input_range_of<std::string> Range>
void PinsManager::validateLabelsLength(const Range& labels)
{
  auto tooLongLabels =
    labels | std::views::filter([](auto&& label) { return label.length() > GPIO_MAX_NAME_SIZE; }) |
    std::views::common;
  // std::ranges::empty is broken
  if (tooLongLabels.begin() != tooLongLabels.end()) {
    auto concatLabels = std::accumulate(std::next(tooLongLabels.begin()),
                                        tooLongLabels.end(),
                                        *tooLongLabels.begin(),
                                        [](auto&& acc, auto&& s) { return acc + ", " + s; });

    throw libopenpresso::Exception{"Pins labels longer than {}: {}", GPIO_MAX_NAME_SIZE, concatLabels};
  }
}

template <input_range_of<std::string> Range>
void PinsManager::validateLabelsUniquness(const Range& labels)
{
  std::unordered_map<std::string, size_t> labelsOccurance;
  std::ranges::for_each(labels, [&labelsOccurance](auto&& label) { ++labelsOccurance[label]; });

  auto duplicatedLabels = labelsOccurance |
                          std::views::filter([](auto&& label) { return label.second > 1; }) |
                          std::views::transform([](auto&& label) { return label.first; });

  if (!std::ranges::empty(duplicatedLabels)) {
    auto concatLabels = std::accumulate(std::next(duplicatedLabels.begin()),
                                        duplicatedLabels.end(),
                                        *duplicatedLabels.begin(),
                                        [](auto&& acc, auto&& s) { return acc + ", " + s; });

    throw libopenpresso::Exception{"Pins labels duplicates found: {}", concatLabels};
  }
}

template <input_range_of<libopenpresso::pin_addr_t> Range>
void PinsManager::validateAddressesUniquness(const Range& addresses) const
{
  std::list<pin_addr_t> duplicates;
  std::unordered_map<pin_id_t, size_t> labelsOccurance;
  std::ranges::copy_if(addresses,
                       std::back_inserter(duplicates),
                       [&labelsOccurance, this](const pin_addr_t& addr) {
                         return ++labelsOccurance[getPinId(addr)] == 2;
                       });

  if (!duplicates.empty()) {
    auto duplicatesView = duplicates | std::views::transform([](auto&& addr) {
                            return std::format("{}:{}", addr.chip.c_str(), addr.pin);
                          });
    auto concatAddrs = std::accumulate(std::next(duplicatesView.begin()),
                                       duplicatesView.end(),
                                       *duplicatesView.begin(),
                                       [](auto&& acc, auto&& s) { return acc + ", " + s; });

    throw libopenpresso::Exception{"Pins addresses duplicates found: {}", concatAddrs};
  }
}
