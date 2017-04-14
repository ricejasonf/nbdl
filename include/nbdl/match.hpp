//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MATCH_HPP
#define NBDL_MATCH_HPP

#include <nbdl/concept/Store.hpp>
#include <nbdl/fwd/match.hpp>
#include <nbdl/get.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/core/default.hpp>
#include <boost/hana/functional/overload_linearly.hpp>
#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace detail
  {
    template <typename Impl, typename Store, typename Fn, typename = void>
    struct matches_identity : std::false_type { };

    template <typename Impl, typename Store, typename Fn>
    struct matches_identity<Impl, Store, Fn,
      decltype(Impl::apply(std::declval<Store>(), std::declval<Fn>()))>
      : std::true_type
    { };
  }

  template<typename Store, typename Key, typename Fn>
  constexpr void match_fn::operator()(Store&& s, Key&& k, Fn&& fn) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = match_impl<Tag>;

    static_assert(
      nbdl::Store<Store>::value
    , "nbdl::match(store, key, fn) requires 'store' to be a Store"
    );

    Impl::apply(
      std::forward<Store>(s)
    , std::forward<Key>(k)
    , std::forward<Fn>(fn)
    );
  };

  template<typename Store, typename Fn>
  constexpr void match_fn::operator()(Store&& s, Fn&& fn) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = match_impl<Tag>;

    static_assert(
      nbdl::Store<Store>::value
    , "nbdl::match(store, fn) requires 'store' to be a Store"
    );

    if constexpr(detail::matches_identity<Impl, Store, Fn>::value)
    {
      Impl::apply(
        std::forward<Store>(s)
      , std::forward<Fn>(fn)
      );
    }
    else
    {
      std::forward<Fn>(fn)(
        std::forward<Store>(s)
      );
    }
  };

  template<typename Tag, bool condition>
  struct match_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  // Use nbdl::get if it is specialized.
  template<typename Tag>
  struct match_impl<Tag, hana::when<!hana::is_default<nbdl::get_impl<Tag>>::value>>
  {
    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      std::forward<Fn>(fn)(
        nbdl::get(std::forward<Store>(s), std::forward<Key>(k))
      );
    }
  };
} // nbdl

#endif
