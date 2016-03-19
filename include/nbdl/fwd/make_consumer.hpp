//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MAKE_CONSUMER_HPP
#define NBDL_FWD_MAKE_CONSUMER_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template <typename T, typename = void>
  struct make_consumer_impl : make_consumer_impl<T, hana::when<true>> { };

  template <typename T>
  struct make_consumer_fn
  {
    template <typename PushFn, typename ...Args>
    constexpr decltype(auto) operator()(PushFn&&, Args&& ...) const;
  };

  template <typename T>
  constexpr make_consumer_fn<T> make_consumer{};
} // nbdl

#endif
