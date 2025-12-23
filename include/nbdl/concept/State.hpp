//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_STATE_HPP
#define NBDL_CONCEPT_STATE_HPP

#include <nbdl/concept/extras.hpp>
#include <nbdl/concept/HasImpl.hpp>

namespace nbdl {
  template <typename Tag>
  struct get_impl : default_impl {
    static constexpr auto apply(...) = delete;

    template <typename S>
    static constexpr decltype(auto) apply(S&& s) {
      return (s);
    }
  };

  template <typename T>
  concept State = HasImpl<T, nbdl::get_impl>;

  template <typename T>
  concept StateAlias = StrongAlias<T> &&
                       State<typename T::value_type>;
}

#endif
