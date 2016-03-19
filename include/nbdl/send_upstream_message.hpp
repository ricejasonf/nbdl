//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SEND_UPSTREAM_MESSAGE_HPP
#define NBDL_SEND_UPSTREAM_MESSAGE_HPP

#include<nbdl/concepts/Provider.hpp>
#include<nbdl/concepts/UpstreamMessage.hpp>
#include<nbdl/fwd/send_upstream_message.hpp>

#include<boost/hana/core/tag_of.hpp>
#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename T, typename Message>
  constexpr auto send_upstream_message_fn::operator()(T&& t, Message&& m) const
  {
    using Tag = hana::tag_of_t<T>;
    using Impl = send_upstream_message_impl<Tag>;
    using MessageTag = hana::tag_of_t<Message>;

    static_assert(nbdl::Provider<T>::value,
      "nbdl::send_upstream_message(t, m) requires 't' to be a Provider");
    static_assert(nbdl::UpstreamMessage<MessageTag>::value,
      "nbdl::send_upstream_message(t, m) requires 'm' to be an UpstreamMessage");

    return Impl::apply(std::forward<T>(t), std::forward<Message>(m));
  };

  template<typename Tag, bool condition>
  struct send_upstream_message_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
