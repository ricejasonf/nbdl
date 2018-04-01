//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ACTOR_TYPE_HPP
#define NBDL_ACTOR_TYPE_HPP

#include <nbdl/fwd/actor_type.hpp>

namespace nbdl::detail
{
  constexpr auto make_actor_initializer = [](auto ctx, auto&& arg)
  {
    return actor_initializer<decltype(ctx), std::decay_t<decltype(arg)>>{
      ctx
    , std::forward<decltype(arg)>(arg)
    };
  };
}

#endif
