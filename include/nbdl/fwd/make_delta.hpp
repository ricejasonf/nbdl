//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_MAKE_DELTA_HPP
#define NBDL_FWD_MAKE_DELTA_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct make_delta_impl : make_delta_impl<T, hana::when<true>> { };

  template <typename T>
  struct make_delta_fn
  {
    template <typename X>
    constexpr auto operator()(X&&) const;

    template <typename X, typename Y>
    constexpr auto operator()(X&&, Y&&) const;
  };

  template <typename T>
  constexpr make_delta_fn<T> make_delta{};
}

#endif
