#ifndef CORE_COMPONENTS_FABRIC_COMPONENT_FABRIC_BASE_HPP
#define CORE_COMPONENTS_FABRIC_COMPONENT_FABRIC_BASE_HPP

#include <cstddef>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <variant>

#include <libopenpresso/config.hpp>
#include <libopenpresso/exception.hpp>
#include <libopenpresso/interfaces/libopenpresso_core.hpp>
#include <libopenpresso/types.hpp>

#include <core/core_private_base.hpp>

#ifdef BOOST_TYPE_ID
#include <string>

#include <boost/type_index.hpp>
template <typename T>
constexpr std::string type_to_string()
{
  return boost::typeindex::type_id<T>().pretty_name();
}
// #define type_to_string(type) boost::typeindex::type_id<type>().pretty_name()
#else
template <typename T>
constexpr const char* type_to_string() noexcept
{
  return typeid(T).name();
}
#endif

namespace libopenpresso
{

template <typename ConfigT, typename MakerT>
concept CanMakeComponentFromConfig = requires(MakerT& m, const ConfigT& t) { m.makeComponent(t); };

template <typename Derived>
class ComponentFabricBase
: protected virtual CorePrivateBase
, protected virtual interfaces::LibopenpressoCore {
  struct result_deducor;
  friend Derived;
  ComponentFabricBase() = default;

public:
  auto getComponent(const component_label_t& label)
  {
    using maker_return_t = result_deducor::type;
    if (auto comp = m_components.find(label); comp != m_components.end()) {
      return std::static_pointer_cast<maker_return_t>(comp->second);
    }

    auto comp = std::visit(makeVisitor(label), findComponentConfig(label));
    m_components[label] = comp;
    return comp;
  }

private:
  auto makeVisitor(const component_label_t& label)
  {
    using maker_return_t = result_deducor::type;
    return [this, &label]<typename ConfigT>(const ConfigT& config) -> std::shared_ptr<maker_return_t> {
      if constexpr (CanMakeComponentFromConfig<ConfigT, Derived>) {
        return static_cast<Derived*>(this)->makeComponent(config);
      }

      throw libopenpresso::Exception{"Component config doesn't match request component type, label "
                                     "\"{}\", config type {}, fabric type {}",
                                     label,
                                     type_to_string<ConfigT>(),
                                     type_to_string<Derived>()};
    };
  }

private:
  std::unordered_map<component_label_t, std::shared_ptr<void>> m_components;
};

template <typename Derived>
struct ComponentFabricBase<Derived>::result_deducor {
  template <typename T>
  struct shared_ptr_element;

  template <typename U>
  struct shared_ptr_element<std::shared_ptr<U>> {
    using type = U;
  };

  template <size_t I = 0>
  static constexpr auto getAcceptableType()
  {
    using alternative_t = std::variant_alternative_t<I, component_config_t>;
    if constexpr (CanMakeComponentFromConfig<alternative_t, Derived>) {
      return std::type_identity<alternative_t>{};
    }
    else {
      return getAcceptableType<I + 1>();
    }
  };

  using acceptable_t = decltype(getAcceptableType())::type;

  static constexpr auto makerSetEnclosure = [](Derived& maker, const acceptable_t& t) {
    return maker.makeComponent(t);
  };

  using ret_ptr_t =
    std::decay_t<std::invoke_result_t<decltype(makerSetEnclosure), Derived&, acceptable_t>>;
  using type = shared_ptr_element<ret_ptr_t>::type;
};

} // namespace libopenpresso

#endif // CORE_COMPONENTS_FABRIC_COMPONENT_FABRIC_BASE_HPP