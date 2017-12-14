//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_GET_PATH_HPP
#define NBDL_GET_PATH_HPP

#include <nbdl/concept/State.hpp>
#include <nbdl/fwd/get_path.hpp>
#include <nbdl/get.hpp>

#include <utility>

namespace nbdl
{
  template<typename State, typename Path>
  constexpr auto get_path_fn::operator()(State&& s, Path&& p) const
  {
    static_assert(
      nbdl::State<State>::value
    , "nbdl::get_path(state, path) requires state must be nbdl::State"
    );
    static_assert(
      hana::Sequence<Path>::value
    , "nbdl::get_path(state, path) requires path must be a hana::Sequence"
    );

    return hana::fold_left(std::forward<Path>(p), std::forward<State>(s), [](auto&& state, auto&& x)
    {
      return nbdl::get(
        std::forward<decltype(state)>(state)
      , std::forward<decltype(x)>(x)
      );
    });
  };
}

#endif
