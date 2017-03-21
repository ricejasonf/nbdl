//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_DETAIL_WRAP_PROMISE_HPP
#define NBDL_FWD_DETAIL_WRAP_PROMISE_HPP

namespace nbdl::detail
{
  // Implicity wraps functions
  // and pipes
  // with the promise interface.
  struct wrap_promise_fn
  {
    template <typename X>
    auto operator()(X&& x) const;
  };

  constexpr wrap_promise_fn wrap_promise{};
}

#endif
