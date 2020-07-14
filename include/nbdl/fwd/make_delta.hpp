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

  template <typename T>
  struct make_delta_fn
  {
    template <BindableSequence X>
    constexpr auto operator()(X&&) const;

    template <BindableSequence X, typename Y>
    constexpr auto operator()(X&&, Y&&) const;
  };

  template <typename T>
  constexpr make_delta_fn<T> make_delta{};
}

#endif
