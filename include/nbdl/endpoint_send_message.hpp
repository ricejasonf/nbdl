//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENDPOINT_SEND_MESSAGE_HPP
#define NBDL_ENDPOINT_SEND_MESSAGE_HPP

#include <nbdl/concept/Endpoint.hpp>
#include <nbdl/fwd/endpoint_send_message.hpp>

namespace nbdl
{
  template <Endpoint Endpoint, typename Message>
  void endpoint_send_message_fn::operator()(Endpoint& endpoint,
                                            Message&& message) const {
    using Tag = hana::tag_of_t<Endpoint>;
    using Impl = endpoint_send_message_impl<Tag>;

    static_assert(
      decltype(hana::typeid_(message) == hana::type_c<typename Endpoint::value_type>)::value
    , "nbdl::endpoint_send_message(endpoint, message) "
      "requires message to of type endpoint::value_type"
    );

    Impl::apply(
      endpoint
    , std::forward<Message>(message)
    );
  };

  template <typename Tag>
  struct endpoint_send_message_impl : hana::default_ {
    template <typename Endpoint, typename Message>
    static void apply(Endpoint& endpoint, Message&& message) {
      endpoint.send_message(std::forward<Message>(message));
    }
  };

  template <EndpointPtr Tag>
  struct endpoint_send_message_impl<Tag> {
    template <typename Endpoint, typename Message>
    static void apply(Endpoint& endpoint, Message&& message) {
      endpoint_send_message(*endpoint, std::forward<Message>(message));
    }
  };
}

#endif
