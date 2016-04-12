//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SEND_UPSTREAM_MESSAGE_HPP
#define NBDL_SEND_UPSTREAM_MESSAGE_HPP

#include<nbdl/concept/Provider.hpp>
#include<nbdl/concept/UpstreamMessage.hpp>
#include<nbdl/fwd/send_upstream_message.hpp>

#include<boost/hana/core/tag_of.hpp>
#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename Provider, typename Message>
  constexpr auto send_upstream_message_fn::operator()(Provider&& p, Message&& m) const
  {
    using Tag = hana::tag_of_t<Provider>;
    using Impl = send_upstream_message_impl<Tag>;
    using MessageTag = hana::tag_of_t<Message>;

    static_assert(nbdl::Provider<Provider>::value,
      "nbdl::send_upstream_message(p, m) requires 'p' to be a Provider");
    static_assert(nbdl::UpstreamMessage<MessageTag>::value,
      "nbdl::send_upstream_message(p, m) requires 'm' to be an UpstreamMessage");

    return Impl::apply(std::forward<Provider>(p), std::forward<Message>(m));
  };

  template<typename Tag, bool condition>
  struct send_upstream_message_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
