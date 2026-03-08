#ifndef HARDWARE_GPIO_PINS_TRANSFORMS_HPP
#define HARDWARE_GPIO_PINS_TRANSFORMS_HPP

#include <ranges>

#include <gpio/pin_data.hpp>
#include <utils/visit_view.hpp>

namespace libopenpresso::gpio
{

static constexpr auto pin_label_view = visit_view(&pin_info_t::label);
static constexpr auto pin_address_view = visit_view(&pin_info_t::pinAddr);
static constexpr auto input_pin_filter =
  std::views::filter([](auto&& pin) { return std::holds_alternative<input_pin_info_t>(pin); }) |
  std::views::transform([](auto&& pin) { return std::get<input_pin_info_t>(pin); });

static constexpr auto output_pin_filter =
  std::views::filter([](auto&& pin) { return std::holds_alternative<output_pin_info_t>(pin); }) |
  std::views::transform([](auto&& pin) { return std::get<output_pin_info_t>(pin); });

static constexpr auto time_sensetive_input_pin_filter =
  input_pin_filter | std::views::filter(&input_pin_info_t::timeCritical);
static constexpr auto non_time_sensetive_input_pin_filter =
  input_pin_filter | std::views::filter([](auto&& pin) { return !pin.timeCritical; });

} // namespace libopenpresso::gpio

#endif // HARDWARE_GPIO_PINS_TRANSFORMS_HPP