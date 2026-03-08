#ifndef UTILS_VISIT_VIEW_HPP
#define UTILS_VISIT_VIEW_HPP

#include <ranges>
#include <utility>
#include <variant>

template <typename T>
constexpr auto visit_view(T&& visitor)
{
  return std::views::transform([visitor = std::forward<T>(visitor)](auto&& entry) {
    return std::visit(visitor, entry);
  });
}

#endif // UTILS_VISIT_VIEW_HPP