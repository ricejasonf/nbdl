//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ENDPOINT_SEND_MESSAGE_HPP
#define NBDL_FWD_ENDPOINT_SEND_MESSAGE_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct endpoint_send_message_impl : endpoint_send_message_impl<T, hana::when<true>> { };

  struct endpoint_send_message_fn
  {
    template <typename Endpoint, typename Message>
    void operator()(Endpoint&, Message&&) const;
  };

  constexpr endpoint_send_message_fn endpoint_send_message{};
}

#endif
