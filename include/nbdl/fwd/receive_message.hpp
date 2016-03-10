//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_RECEIVE_MESSAGE_HPP
#define NBDL_FWD_RECEIVE_MESSAGE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename T, typename = void>
  struct receive_message_impl : receive_message_impl<T, hana::when<true>> { };

  struct receive_message_fn
  {
    template<typename R, typename Message>
    constexpr auto operator()(R&&, Message&&) const;
  };

  constexpr receive_message_fn receive_message{};
} // nbdl

#endif

