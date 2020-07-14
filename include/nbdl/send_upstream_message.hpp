//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SEND_UPSTREAM_MESSAGE_HPP
#define NBDL_SEND_UPSTREAM_MESSAGE_HPP

#include <nbdl/concept/Producer.hpp>
#include <nbdl/concept/UpstreamMessage.hpp>
#include <nbdl/fwd/send_upstream_message.hpp>

#include <boost/hana/core/tag_of.hpp>

namespace nbdl {
  template<Producer Producer, UpstreamMessage Message>
  constexpr auto send_upstream_message_fn::operator()(Producer&& p,
                                                      Message&& m) const {
    using Tag = hana::tag_of_t<Producer>;
    using Impl = send_upstream_message_impl<Tag>;

    return Impl::apply(std::forward<Producer>(p), std::forward<Message>(m));
  };
}

#endif
