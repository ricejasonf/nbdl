//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_HOLD_LAZY_HPP
#define NBDL_FWD_HOLD_LAZY_HPP

namespace nbdl
{
  template <typename ...T>
  struct lazy_args_t { };

  template <typename ...T>
  lazy_args_t<T...> lazy_args{};

  struct hold_lazy_fn
  {
    template <typename F>
    auto operator()(F&& f) const;

    template <typename F, typename ...Args>
    auto operator()(F&& f, lazy_args_t<Args...>) const;
  };

  constexpr hold_lazy_fn hold_lazy{};
}

namespace nbdl::detail
{
  // This tag is used in promise_join
  // to construct the promise on the fly
  // once we know the type of its Resolver.
  struct make_lazy_holder_tag { };
}
#endif
