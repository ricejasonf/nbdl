//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_VARIANT_HPP
#define NBDL_FWD_BIND_VARIANT_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct bind_variant_impl : bind_variant_impl<T, hana::when<true>> { };

  struct bind_variant_fn
  {
    // A variant is just like a sequence
    // when we know the type_id.
    template<typename BindableVariant, typename BindFn>
    constexpr void operator()(int const type_id, BindableVariant&&, BindFn&&) const;

    template<typename BindableVariant, typename BindFn>
    constexpr void operator()(BindableVariant const&, BindFn&&) const;
  };

  constexpr bind_variant_fn bind_variant{};
} // nbdl

#endif

