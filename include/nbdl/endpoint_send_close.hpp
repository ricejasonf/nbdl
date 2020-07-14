//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENDPOINT_SEND_CLOSE_HPP
#define NBDL_ENDPOINT_SEND_CLOSE_HPP

#include <nbdl/concept/Endpoint.hpp>
#include <nbdl/concept/HasImpl.hpp>
#include <nbdl/fwd/endpoint_send_close.hpp>

namespace nbdl {
  template <Endpoint Endpoint>
  void endpoint_send_close_fn::operator()(Endpoint& endpoint) const {
    using Tag = hana::tag_of_t<Endpoint>;
    using Impl = endpoint_send_close_impl<Tag>;

    Impl::apply(endpoint);
  };

  template <typename Tag>
  struct endpoint_send_close_impl : default_impl {
    template <typename Endpoint>
    static void apply(Endpoint& endpoint) {
      endpoint.send_close();
    }
  };

  template <EndpointPtr Tag>
  struct endpoint_send_close_impl<Tag> {
    template <typename Endpoint>
    static void apply(Endpoint& endpoint) {
      endpoint_send_close(*endpoint);
    }
  };
}

#endif
