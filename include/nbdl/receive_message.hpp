//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_RECEIVE_MESSAGE_HPP
#define NBDL_RECEIVE_MESSAGE_HPP

#include<nbdl/concepts/MessageReceiver.hpp>
#include<nbdl/fwd/receive_message.hpp>

#include<boost/hana/core/tag_of.hpp>
#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename R, typename Message>
  constexpr auto receive_message_fn::operator()(R&& r, Message&& m) const
  {
    using Tag = hana::tag_of_t<R>;
    using Impl = receive_message_impl<Tag>;

    static_assert(nbdl::MessageReceiver<R>::value,
      "nbdl::receive_message(r, m) requires 'r' to be a MessageReceiver");

    return Impl::apply(std::forward<R>(r), std::forward<Message>(m));
  };

  template<typename Tag, bool condition>
  struct receive_message_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
