//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_GET_HPP
#define NBDL_FWD_GET_HPP

#include <nbdl/concept/State.hpp>

namespace nbdl {
  struct get_fn {
    template <typename State>
    constexpr decltype(auto) operator()(State&&) const;

    template <State State, typename Key>
    constexpr decltype(auto) operator()(State&&, Key&&) const;
  };

  constexpr get_fn get{};
}

#endif
