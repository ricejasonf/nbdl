//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MATCH_PATH_HPP
#define NBDL_FWD_MATCH_PATH_HPP

namespace nbdl
{
  struct match_path_fn
  {
    template<typename Store, typename Path, typename ...Fn>
    constexpr void operator()(Store&&, Path&&, Fn&&...) const;
  };

  constexpr match_path_fn match_path{};
} // nbdl

#endif
