//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_SEND_DOWNSTREAM_MESSAGE_HPP
#define NBDL_FWD_SEND_DOWNSTREAM_MESSAGE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct send_downstream_message_impl : send_downstream_message_impl<T, hana::when<true>> { };

  struct send_downstream_message_fn
  {
    template<typename T, typename Message>
    constexpr auto operator()(T&&, Message&&) const;
  };

  constexpr send_downstream_message_fn send_downstream_message{};
} // nbdl

#endif

