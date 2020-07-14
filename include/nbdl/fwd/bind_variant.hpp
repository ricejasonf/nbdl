//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_VARIANT_HPP
#define NBDL_FWD_BIND_VARIANT_HPP

#include <nbdl/concept/BindableVariant.hpp>

namespace nbdl {
  struct bind_variant_fn {
    // A variant is just like a sequence
    // when we know the type_id.
    template <BindableVariant BindableVariant, typename BindFn>
    constexpr auto operator()(int const type_id, BindableVariant&&, BindFn&&) const;

    template <BindableVariant BindableVariant, typename BindFn>
    constexpr auto operator()(BindableVariant const&, BindFn&&) const;
  };

  constexpr bind_variant_fn bind_variant{};

  // bind_variant_types
  // Customization point for specifying the types that an instance of a variant supports
  // It must be set to an instance of hana::types and have at least one type
  template <typename>
  auto bind_variant_types = 42;
}

#endif

