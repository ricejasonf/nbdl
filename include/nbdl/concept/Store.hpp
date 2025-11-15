//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_STORE_HPP
#define NBDL_CONCEPT_STORE_HPP

#include <nbdl/concept/HasImpl.hpp>
#include <nbdl/concept/extras.hpp>
#include <utility>

namespace nbdl {
  template <typename Tag>
  struct match_impl : hana::default_ {
    static constexpr void apply(...) = delete;

    template <typename T, typename Fn>
    static constexpr void apply(T&& t, Fn&& fn) {
      std::forward<Fn>(fn)(std::forward<T>(t));
    }
  };

  template <typename T>
  concept Store = HasImpl<T, nbdl::match_impl>;

  template <typename T>
  concept StoreAlias = StrongAlias<T> &&
                       Store<typename T::value_type>;
}

#endif
