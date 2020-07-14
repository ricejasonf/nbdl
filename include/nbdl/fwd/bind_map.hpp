//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_MAP_HPP
#define NBDL_FWD_BIND_MAP_HPP

#include <nbdl/concept/BindableMap.hpp>

namespace nbdl {
  struct bind_map_fn
  {
    template <BindableMap BindableMap, typename BindKeyValueFn>
    constexpr auto operator()(BindableMap&& s, BindKeyValueFn&& f) const;
  };

  constexpr bind_map_fn bind_map{};
}

#endif


