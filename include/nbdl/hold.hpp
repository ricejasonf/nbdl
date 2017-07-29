//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_HOLD_HPP
#define NBDL_HOLD_HPP

#include <nbdl/promise.hpp>

#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace detail
  {
    template <typename T>
    struct holder
    {
      T value;

      template <typename Resolve, typename ...Args>
      void operator()(Resolve& resolve, Args&&...) // mutable
      {
        resolve(value);
      }
    };
  }

  // Kind of like `always` but the held value is mutable
  constexpr auto hold = [](auto&& value)
  {
    using T = std::decay_t<decltype(value)>;
    return nbdl::promise(detail::holder<T>{std::forward<decltype(value)>(value)});
  };
}

#endif
