//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BIND_VARIANT_HPP
#define NBDL_BIND_VARIANT_HPP

#include<nbdl/fwd/bind_variant.hpp>

#include<nbdl/concept/BindableVariant.hpp>
#include<nbdl/detail/filter_empties.hpp>

#include<boost/hana/core/tag_of.hpp>
#include<type_traits>
#include<utility>

namespace nbdl
{
  template<typename BindableVariant, typename BindFn>
  constexpr auto bind_variant_fn::operator()(int const type_id, BindableVariant&& v, BindFn&& f) const
  {
    using Tag = hana::tag_of_t<BindableVariant>;
    using Impl = bind_variant_impl<Tag>;

    static_assert(nbdl::BindableVariant<BindableVariant>::value,
      "nbdl::bind_variant(int, v, fn) requires 'v' to be a BindableVariant");

    return Impl::apply(
      type_id,
      std::forward<BindableVariant>(v),
      detail::filter_empties_fn<std::decay_t<BindFn>>{std::forward<BindFn>(f)}
    );
  };

  template<typename BindableVariant, typename BindFn>
  constexpr auto bind_variant_fn::operator()(BindableVariant const& v, BindFn&& f) const
  {
    using Tag = hana::tag_of_t<BindableVariant>;
    using Impl = bind_variant_impl<Tag>;

    static_assert(nbdl::BindableVariant<BindableVariant>::value,
      "nbdl::bind_variant(v, fn) requires 'v' to be a BindableVariant");

    return Impl::apply(
      v,
      detail::filter_empties_fn<std::decay_t<BindFn>>{std::forward<BindFn>(f)}
    );
  };

  template<typename Tag, bool condition>
  struct bind_variant_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
