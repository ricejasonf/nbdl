//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SEND_DOWNSTREAM_MESSAGE_HPP
#define NBDL_SEND_DOWNSTREAM_MESSAGE_HPP

#include<nbdl/concept/Provider.hpp>
#include<nbdl/concept/DownstreamMessage.hpp>
#include<nbdl/concept/StateConsumer.hpp>
#include<nbdl/fwd/send_downstream_message.hpp>

#include<boost/hana/core/tag_of.hpp>
#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename T, typename Message>
  constexpr auto send_downstream_message_fn::operator()(T&& t, Message&& m) const
  {
    using Tag = hana::tag_of_t<T>;
    using Impl = send_downstream_message_impl<Tag>;
    using MessageTag = hana::tag_of_t<Message>;

    static_assert(nbdl::Provider<T>::value,
      "nbdl::send_downstream_message(t, m) requires 't' to be a Provider");
    static_assert(nbdl::DownstreamMessage<MessageTag>::value,
      "nbdl::send_downstream_message(t, m) requires 'm' to be an DownstreamMessage");

    return Impl::apply(std::forward<T>(t), std::forward<Message>(m));
  };

  template<typename Tag, bool condition>
  struct send_downstream_message_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  template<typename Tag>
  struct send_downstream_message_impl<Tag, hana::when<nbdl::StateConsumer<Tag>::value>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif

