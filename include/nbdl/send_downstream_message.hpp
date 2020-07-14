//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SEND_DOWNSTREAM_MESSAGE_HPP
#define NBDL_SEND_DOWNSTREAM_MESSAGE_HPP

#include <nbdl/concept/Consumer.hpp>
#include <nbdl/concept/DownstreamMessage.hpp>
#include <nbdl/fwd/send_downstream_message.hpp>

#include <boost/hana/core/tag_of.hpp>

namespace nbdl
{
  template <Consumer T, DownstreamMessage Message>
  constexpr auto send_downstream_message_fn::operator()(T&& t,
                                                        Message&& m) const {
    using Tag = hana::tag_of_t<T>;
    using Impl = send_downstream_message_impl<Tag>;

    return Impl::apply(std::forward<T>(t), std::forward<Message>(m));
  };
}

#endif

