//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_THROWABLE_TRANSFORM_HPP
#define NBDL_THROWABLE_TRANSFORM_HPP

#include <nbdl/promise.hpp>

#include <utility>

namespace nbdl
{
  // Wraps a function in a promise that
  // catches exceptions and "rejects" them.
  struct throwable_transform_fn
  {
    template <typename Fn>
    auto operator()(Fn&& fn) const
    {
      return nbdl::promise([fn](auto&& resolver, auto&& ...args) noexcept
      {
        using Return = decltype(fn(std::forward<decltype(args)>(args)...));
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
      });
    }
  };

  constexpr throwable_transform_fn throwable_transform{};
}

#endif
