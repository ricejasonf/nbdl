//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_VARIANT_HOLDER_HPP
#define NBDL_VARIANT_HOLDER_HPP


#include <nbdl/concept/BindableVariant.hpp>
#include <nbdl/concept/Store.hpp>
#include <nbdl/variant.hpp>
#include <boost/hana/functional/overload.hpp>
#include <boost/mp11/algorithm.hpp>
#include <variant>

namespace nbdl {
namespace hana = boost::hana;
namespace mp11 = boost::mp11;

struct variant_holder_tag { };
inline constexpr struct variant_index_t { } variant_index;
inline constexpr struct variant_value_t { } variant_value;

// Represent the product of a variant value and the index of its alternative.
// VariantTy has to work with std::visit, std::variant_alternative, ...
// which is unfortunately limited to std::variant.
template <typename... Tn>
class variant_holder {
public:
  using hana_tag = nbdl::variant_holder_tag;
  using value_type = detail::variant<Tn ...>;
  // .value has the properties of the sum type.
  value_type value;
};

template<>
struct match_impl<variant_holder_tag> {
  template <typename Store, typename Fn>
  static constexpr void apply(Store&& s, variant_index_t, Fn&& fn) {
    std::forward<Fn>(fn)(std::forward<Store>(s).value.value_.index());
  }

  template <typename Store, typename Fn>
  static constexpr void apply(Store&& s, variant_value_t, Fn&& fn) {
    std::visit(std::forward<Fn>(fn), std::forward<Store>(s).value.value_);
  }
};

template <>
struct bind_variant_impl<nbdl::variant_holder_tag> {
  template <typename VariantHolder, typename BindFn>
  static void apply(int type_id, VariantHolder&& v, BindFn&& f) {
    using VariantTy = std::remove_cvref_t<VariantHolder>;
    constexpr size_t variant_size = std::variant_size_v<VariantTy>;

    mp11::mp_with_index<variant_size + 1>(type_id, [&](auto index) {
      using AltType = std::variant_alternative_t<index(), VariantTy>;
      AltType value{};
      std::forward<BindFn>(f)(type_id, value);
      v = value;
    });
  }

  template <typename VariantHolder, typename BindFn>
  static void apply(VariantHolder&& v, BindFn&& f) {
    v.match([&](auto const& value) {
      f(v.index(), value);
    });
  }
};
}  // namespace nbdl

#endif
