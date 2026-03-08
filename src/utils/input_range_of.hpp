#ifndef UTILS_INPUT_RANGE_OF_HPP
#define UTILS_INPUT_RANGE_OF_HPP

#include <concepts>
#include <ranges>

template <typename T, typename Elem>
concept input_range_of =
  std::ranges::input_range<T> && std::convertible_to<std::ranges::range_value_t<T>, Elem>;

#endif // UTILS_INPUT_RANGE_OF_HPP
