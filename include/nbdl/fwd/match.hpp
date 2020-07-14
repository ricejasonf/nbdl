//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MATCH_HPP
#define NBDL_FWD_MATCH_HPP

#include <nbdl/concept/Store.hpp>

namespace nbdl {
  struct match_fn {
    template<Store Store, typename Key, typename Fn>
    constexpr void operator()(Store&&, Key&&, Fn&&) const;

    template<Store Store, typename Fn>
    constexpr void operator()(Store&&, Fn&&) const;
  };

  constexpr match_fn match{};

  //
  // Below is a collection of named visitors
  // that Stores could overload within `match`
  // to make optimizations based on their
  // semantics.
  //

  //
  // match_when<T>(f) - Calls f only when the Visited value is of type T.
  //                    Otherwise, it does nothing.

  struct match_when_tag { };

  template <typename T, typename F>
  struct match_when_t {
    using hana_tag = match_when_tag;
    using type = T;
    F f;

    template <typename X>
    constexpr void operator()(X&& x) const;
  };

  template <typename T>
  struct match_when_fn {
    template <typename F>
    constexpr auto operator()(F&&) const;

    template <Store Store, typename F>
    constexpr auto operator()(Store&&, F&&) const;

    template <Store Store, typename Key, typename F>
    constexpr auto operator()(Store&&, Key&&, F&&) const;
  };

  template <typename T>
  constexpr match_when_fn<T> match_when{};

  //
  // mapped_overload<T> - visitor that takes a `hana::Searchable` with
  //                      `hana::type` as keys to choose an overload.
  //                      If the key is not found then the Otherwise function
  //                      is invoked with the visited value
  //

  struct mapped_overload_tag { };

  template <typename Searchable, typename Otherwise>
  struct mapped_overload_t
  {
    using hana_tag = mapped_overload_tag;

    Searchable map;
    Otherwise otherwise;

    template <typename X>
    constexpr void operator()(X&&) const;
  };

  struct mapped_overload_fn
  {
    template <typename Searchable, typename Otherwise>
    constexpr auto operator()(Searchable&&, Otherwise&&) const;
  };

  constexpr mapped_overload_fn mapped_overload{};
}

#endif
