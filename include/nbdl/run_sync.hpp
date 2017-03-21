//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_RUN_SYNC_HPP
#define NBDL_RUN_SYNC_HPP

#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  struct run_sync_fn
  {
    // Promise should be an lvalue reference
    template <typename Promise, typename ...Args>
    void operator()(Promise& promise, Args&&... args) const
    {
      promise.resolve(
        nbdl::catch_([](auto&& x1, auto&& ...) {
          static_assert(
            std::is_same<std::decay_t<decltype(x1)>, void>::value
          , "Unhandled Rejection!"
          );
        })
      , std::forward<Args>(args)...
      );
    }
  };

  constexpr run_sync_fn run_sync{};
}

#endif
