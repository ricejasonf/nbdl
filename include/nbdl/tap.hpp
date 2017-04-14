//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TAP_HPP
#define NBDL_TAP_HPP

#include <nbdl/promise.hpp>

#include <utility>

namespace nbdl
{
  struct tap_fn
  {
    template <typename Fn>
    auto operator()(Fn&& fn) const
    {
      return nbdl::promise([=](auto&& resolver, auto&& ...args)
      {
        fn(args...);
        resolver.resolve(std::forward<decltype(args)>(args)...);
      });
    }
  };

  constexpr tap_fn tap{};
}

#endif
