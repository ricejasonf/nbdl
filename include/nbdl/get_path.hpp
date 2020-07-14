//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_GET_PATH_HPP
#define NBDL_GET_PATH_HPP

#include <nbdl/concept/State.hpp>
#include <nbdl/concept/Path.hpp>
#include <nbdl/fwd/get_path.hpp>
#include <nbdl/get.hpp>

#include <boost/hana/concept/sequence.hpp>
#include <boost/hana/fold_left.hpp>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  template<State State, Path Path>
  constexpr decltype(auto) get_path_fn::operator()(State&& s, Path&& p) const {
    return hana::fold_left(
      std::forward<Path>(p)
    , std::forward<State>(s)
    , [](auto&& state, auto&& x) -> decltype(auto) {
        return nbdl::get(
          std::forward<decltype(state)>(state)
        , std::forward<decltype(x)>(x)
        );
      });
  };
}

#endif
