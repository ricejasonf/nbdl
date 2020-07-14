//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MATCH_PATH_HPP
#define NBDL_FWD_MATCH_PATH_HPP

#include <nbdl/concept/Store.hpp>
#include <nbdl/concept/Path.hpp>

namespace nbdl {
  struct match_path_fn {
    template <typename Store, Path Path, typename ...Fns>
      requires (!EmptyPath<Path>)
    constexpr void operator()(Store&&, Path&&, Fns&&...) const;

    template <Store Store, EmptyPath Path, typename ...Fns>
    constexpr void operator()(Store&&, Path&&, Fns&&...) const;

    template <typename T, EmptyPath Path, typename ...Fns>
    constexpr void operator()(T&&, Path&&, Fns&&...) const;
  };

  constexpr match_path_fn match_path{};
}

#endif
