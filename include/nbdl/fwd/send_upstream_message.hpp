//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_SEND_UPSTREAM_MESSAGE_HPP
#define NBDL_FWD_SEND_UPSTREAM_MESSAGE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename T, typename = void>
  struct send_upstream_message_impl : send_upstream_message_impl<T, hana::when<true>> { };

  struct send_upstream_message_fn
  {
    template<typename T, typename Message>
    constexpr auto operator()(T&&, Message&&) const;
  };

  constexpr send_upstream_message_fn send_upstream_message{};
} // nbdl

#endif

