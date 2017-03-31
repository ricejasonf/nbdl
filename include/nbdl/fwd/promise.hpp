//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_PROMISE_HPP
#define NBDL_FWD_PROMISE_HPP

namespace nbdl
{
  struct promise_tag { };

  template <typename Fn>
  struct promise_t;

  struct promise_fn
  {
    template <typename Fn>
    auto operator()(Fn&& fn) const;
  };

  constexpr promise_fn promise;
}

#endif
