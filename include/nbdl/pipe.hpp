//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PIPE_HPP
#define NBDL_PIPE_HPP

#include <nbdl/catch.hpp>
#include <nbdl/detail/pipe_rejection.hpp>
#include <nbdl/promise.hpp>

#include <boost/hana/core/is_a.hpp>
#include <boost/hana/fold_right.hpp>
#include <boost/hana/functional/overload_linearly.hpp>
#include <boost/hana/tuple.hpp>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    struct pipe_tag {};

    template <typename ResolveFn>
    struct resolver
    {
      ResolveFn resolve;

      resolver(ResolveFn r)
        : resolve(r)
      { }

      template <typename ...Args>
      void operator()(Args&& ... args) const
      {
        resolve(std::forward<Args>(args)...);
      }

      template <typename ...Args>
      void reject(Args ... args) const
      {
        resolve(detail::pipe_rejection{}, std::forward<Args>(args)...);
      }
    };

    template <typename ResolveFn>
    auto make_resolver(ResolveFn resolve)
    {
      return resolver<ResolveFn>(resolve);
    }

    // Implicity wraps functions
    // with the promise interface.
    struct wrap_promise_fn
    {
      template <typename Fn>
      auto operator()(Fn&& fn) const
      {
        using DecayedFn = std::decay_t<Fn>;
        if constexpr(hana::is_a<detail::promise_tag, DecayedFn>()
                  || hana::is_a<detail::pipe_tag, DecayedFn>()
                  || hana::is_a<detail::catch_tag, DecayedFn>())
        {
          return std::forward<Fn>(fn);
        }
        else
        {
          return nbdl::promise(
            [fn](auto&& resolve, auto&&... args)
            {
              using Return = decltype(fn(std::forward<decltype(args)>(args)...));
              // handle void edge case
              if constexpr(std::is_void<Return>::value)
              {
                fn(std::forward<decltype(args)>(args)...);
                resolve();
              }
              else
              {
                resolve(fn(std::forward<decltype(args)>(args)...));
              }
            }
          );
        }
      }
    };

    constexpr wrap_promise_fn wrap_promise{};

    struct default_end_pipe_fn
    {
      template <typename ...Args>
      void operator()(Args&& ...) const
      {
        static_assert(!is_pipe_rejection<std::decay_t<Args>...>::value, "Unhandled Rejection!");
      }
    };

    template <typename Handlers>
    struct pipe_t
    {
      std::shared_ptr<Handlers> handlers;

      using hana_tag = pipe_tag;

      pipe_t(Handlers&& h)
        : handlers(std::make_shared<Handlers>(std::move(h)))
      { }

      template <typename EndPipeFn, typename ...InvokeArgs>
      decltype(auto) invoke_impl(EndPipeFn&& end_pipe, InvokeArgs&& ...invoke_args)
      {
        hana::fold_right(
          *handlers,
          [=, keep_alive = handlers](auto&& ...a)
          {
            end_pipe(std::forward<decltype(a)>(a)...);
          },
          [](auto& x, auto&& state)
          {
            return [resolve = detail::make_resolver(state), &x](auto&& ...results)
            {
              if constexpr(!is_pipe_rejection<std::decay_t<decltype(results)>...>::value)
              {
                // nested pipe
                if constexpr(hana::is_a<pipe_tag, decltype(x)>())
                {
                  x.invoke_impl(
                    resolve,
                    std::forward<decltype(results)>(results)...
                  );
                }
                else if constexpr(hana::is_a<promise_tag, decltype(x)>())
                {
                  // promise
                  x(
                    resolve,
                    std::forward<decltype(results)>(results)...
                  );
                }
              }
              // handle pipe rejections
              else if constexpr(hana::is_a<catch_tag, decltype(x)>())
              {
                // try this catch or move on the the next one
                hana::overload_linearly(std::ref(x), resolve)(std::forward<decltype(results)>(results)...);
              }
              else
              {
                // not a catch so move to the next one
                resolve(std::forward<decltype(results)>(results)...);
              }
            };
          })(std::forward<InvokeArgs>(invoke_args)...);

        return *this;
      }

      template <typename ...Args>
      decltype(auto) operator()(Args&& ...args)
      {
        return invoke_impl(default_end_pipe_fn{}, std::forward<Args>(args)...);
      }
    };
  }

  struct pipe_fn
  {
    template <typename ...Args>
    auto operator()(Args&&... args) const
    {
      using Tuple = decltype(
        hana::make_tuple(detail::wrap_promise(std::forward<Args>(args))...)
      );

      return detail::pipe_t<Tuple>{
        hana::make_tuple(detail::wrap_promise(std::forward<Args>(args))...)
      };
    }
  };

  constexpr pipe_fn pipe{};
}

#endif
