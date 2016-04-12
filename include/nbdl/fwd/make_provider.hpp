//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MAKE_PROVIDER_HPP
#define NBDL_FWD_MAKE_PROVIDER_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct make_provider_impl : make_provider_impl<T, hana::when<true>> { };

  template <typename T>
  struct make_provider_fn
  {
    template <typename PushApi, typename ...Args>
    constexpr decltype(auto) operator()(PushApi&&, Args&& ...) const;
  };

  template <typename T>
  constexpr make_provider_fn<T> make_provider{};
} // nbdl

#endif
