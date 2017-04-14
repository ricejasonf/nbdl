//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_RUN_SYNC_HPP
#define NBDL_RUN_SYNC_HPP

#include <nbdl/detail/promise_join.hpp>
#include <nbdl/promise.hpp>

#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    struct promise_end_sync
    {
      using hana_tag = promise_tag;

      template <typename ...Args>
      void resolve(Args&&...)
      { }

      template <typename Arg1, typename ...Args>
      void reject(Arg1&&, Args&&...)
      {
        static_assert(
          std::is_void<Arg1>::value
        , "Unhandled Rejection!"
        );
      }

      template <typename ...Args>
      void terminate(Args&&...)
      { }
    };
  }

  struct run_sync_fn
  {
    // Promise should be an lvalue reference
    template <typename Promise, typename ...Args>
    void operator()(Promise&& promise, Args&&... args) const
    {
      detail::promise_join(
        std::forward<Promise>(promise)
      , detail::promise_end_sync{}
      ).resolve(std::forward<Args>(args)...);
    }
  };

  constexpr run_sync_fn run_sync{};
}

#endif
