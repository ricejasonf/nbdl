//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_RUN_ASYNC_HPP
#define NBDL_RUN_ASYNC_HPP

#include <nbdl/detail/promise_join.hpp>
#include <nbdl/detail/wrap_promise.hpp>
#include <nbdl/fwd/promise.hpp>

#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace detail
  {
    template <typename End>
    struct promise_end
    {
      using hana_tag = promise_tag;

      End* end;

      promise_end(End* e)
        : end(e)
      { }

      template <typename ...Args>
      void resolve(Args&&...)
      {
        delete end;
      }

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
      {
        delete end;
      }
    };

    template <typename InputPromise>
    struct shared_end_pipe
    {
      using Promise = decltype(detail::promise_join(
        std::declval<InputPromise>()
      , std::declval<promise_end<shared_end_pipe>>()
      ));
      
      // Holds joined promise for async pipes
      Promise promise;

      shared_end_pipe(InputPromise const& p)
        : promise(promise_join(p, promise_end<shared_end_pipe>(this)))
      { }

      shared_end_pipe(InputPromise&& p)
        : promise(promise_join(std::move(p), promise_end<shared_end_pipe>(this)))
      { }

      shared_end_pipe(shared_end_pipe const&) = delete;
    };
  }

  struct run_async_fn
  {
    template <typename P, typename ...Args>
    void operator()(P&& promise, Args&&... args) const
    {
      using Promise = std::decay_t<P>;

      auto shared_promise = new detail::shared_end_pipe<Promise>(
        std::forward<P>(promise)
      );

      shared_promise->promise.resolve(std::forward<Args>(args)...);
    }
  };

  constexpr run_async_fn run_async{};
}

#endif
