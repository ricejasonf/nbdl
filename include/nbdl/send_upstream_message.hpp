//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SEND_UPSTREAM_MESSAGE_HPP
#define NBDL_SEND_UPSTREAM_MESSAGE_HPP

#include<nbdl/concept/Producer.hpp>
#include<nbdl/concept/UpstreamMessage.hpp>
#include<nbdl/fwd/send_upstream_message.hpp>

#include<boost/hana/core/tag_of.hpp>
#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename Producer, typename Message>
  constexpr auto send_upstream_message_fn::operator()(Producer&& p, Message&& m) const
  {
    using Tag = hana::tag_of_t<Producer>;
    using Impl = send_upstream_message_impl<Tag>;

    static_assert(nbdl::Producer<Producer>::value,
      "nbdl::send_upstream_message(p, m) requires 'p' to be a Producer");
    static_assert(nbdl::UpstreamMessage<Message>::value,
      "nbdl::send_upstream_message(p, m) requires 'm' to be an UpstreamMessage");

    return Impl::apply(std::forward<Producer>(p), std::forward<Message>(m));
  };

  template<typename Tag, bool condition>
  struct send_upstream_message_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
