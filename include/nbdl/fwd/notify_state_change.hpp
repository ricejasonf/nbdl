//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_NOTIFY_STATE_CHANGE_HPP
#define NBDL_FWD_NOTIFY_STATE_CHANGE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct notify_state_change_impl : notify_state_change_impl<T, hana::when<true>> { };

  struct notify_state_change_fn
  {
    template<typename StateConsumer, typename Path>
    constexpr auto operator()(StateConsumer&&, Path&&) const;
  };

  constexpr notify_state_change_fn notify_state_change{};
} // nbdl

#endif

