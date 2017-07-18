//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_RUN_ASYNC_LOOP_HPP
#define NBDL_RUN_ASYNC_LOOP_HPP

#include <nbdl/detail/promise_join.hpp>
#include <nbdl/fwd/promise.hpp>
#include <nbdl/run_async.hpp>

#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace detail
  {
    template <typename End>
    struct promise_end_loop
    {
      using hana_tag = promise_tag;

      End* end;

      promise_end_loop(End* e)
        : end(e)
      { }

      template <typename ...Args>
      void operator()(Args&&... args)
      {
        // Repeat the sequence from the beginning
        this->resolve(std::forward<Args>(args)...);
      }

      template <typename ...Args>
      void resolve(Args&&... args)
      {
        // Repeat the sequence from the beginning
        end->promise.resolve(std::forward<Args>(args)...);
      }

      template <typename ...Args>
      void reject(Args&&...)
      {
        static_assert(
          sizeof...(Args) > 9000
        , "Unhandled Rejection!"
        );
      }

      template <typename ...Args>
      void terminate(Args&&...)
      {
        delete end;
      }
    };

    template <typename InputPromise>
    struct shared_end_pipe_loop
    {
      using Promise = decltype(detail::promise_join(
        std::declval<InputPromise>()
      , std::declval<promise_end_loop<shared_end_pipe_loop>>()
      ));

      // Holds joined promise for async pipes
      Promise promise;

      shared_end_pipe_loop(InputPromise const& p)
        : promise(promise_join(p, promise_end_loop<shared_end_pipe_loop>(this)))
      { }

      shared_end_pipe_loop(InputPromise&& p)
        : promise(promise_join(std::move(p), promise_end_loop<shared_end_pipe_loop>(this)))
      { }

      shared_end_pipe_loop(shared_end_pipe_loop const&) = delete;
    };
  }

  struct run_async_loop_fn
  {
    template <typename P, typename ...Args>
    void operator()(P&& promise, Args&&... args) const
    {
      using Promise = std::decay_t<P>;

      auto shared_promise = new detail::shared_end_pipe_loop<Promise>(
        std::forward<P>(promise)
      );

      shared_promise->promise.resolve(std::forward<Args>(args)...);
    }
  };

  constexpr run_async_loop_fn run_async_loop{};
}

#endif
