//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PROMISE_HPP
#define NBDL_PROMISE_HPP

#include <nbdl/fwd/promise.hpp>

#include <utility>

namespace nbdl
{
  template <typename Fn>
  struct promise_t
  {
    using hana_tag = promise_tag;

    Fn fn;

    template <typename ...Args>
    void resolve(Args&&... args) noexcept
    {
      fn(std::forward<Args>(args)...);
    }

    template <typename Resolver, typename ...Args>
    void reject(Resolver&& resolver, Args&&... args) noexcept
    {
      // propagate rejection
      std::forward<Resolver>(resolver).reject(std::forward<Args>(args)...);
    }
  };

  template <typename Fn>
  auto promise_fn::operator()(Fn&& fn) const
  {
    return promise_t<std::decay_t<Fn>>{std::forward<Fn>(fn)};
  }
}

#endif
