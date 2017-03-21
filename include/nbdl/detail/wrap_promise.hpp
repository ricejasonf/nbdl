//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_WRAP_PROMISE_HPP
#define NBDL_DETAIL_WRAP_PROMISE_HPP

#include <nbdl/fwd/detail/wrap_promise.hpp>

#include <nbdl/concept/Resolver.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/fwd/detail/promise_join.hpp>

#include <stdexcept>
#include <utility>

namespace nbdl::detail
{
  namespace hana = boost::hana;

  // Implicity wraps functions
  // and pipes
  // with the promise interface.
  template <typename X>
  auto wrap_promise_fn::operator()(X&& x) const
  {
    if constexpr(nbdl::Resolver<X>())
    {
      return std::forward<X>(x);
    }
    else
    {
      auto temp = std::forward<X>(x); //??
      return nbdl::promise([fn = std::move(temp)](auto&& resolver, auto&& ...args)
      {
        using Return = decltype(fn(std::forward<decltype(args)>(args)...));
        if constexpr(noexcept(fn(std::forward<decltype(args)>(args)...)))
        {
          // handle void edge case
          if constexpr(std::is_void<Return>::value)
          {
            fn(std::forward<decltype(args)>(args)...);
            resolver.resolve();
          }
          else
          {
            resolver.resolve(fn(std::forward<decltype(args)>(args)...));
          }
        }
        else
        {
          try
          {
            // handle void edge case
            if constexpr(std::is_void<Return>::value)
            {
              fn(std::forward<decltype(args)>(args)...);
              resolver.resolve();
            }
            else
            {
              resolver.resolve(fn(std::forward<decltype(args)>(args)...));
            }
          }
          catch (std::exception const& e)
          { resolver.reject(e); }
          catch (...)
          { resolver.reject(std::runtime_error("Unknown exception")); }
        }
      });
    }
  };
}

#endif
