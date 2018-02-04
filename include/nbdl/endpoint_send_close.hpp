//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENDPOINT_SEND_CLOSE_HPP
#define NBDL_ENDPOINT_SEND_CLOSE_HPP

#include <nbdl/concept/Endpoint.hpp>
#include <nbdl/fwd/endpoint_send_close.hpp>

namespace nbdl
{
  template <typename Endpoint>
  void endpoint_send_close_fn::operator()(Endpoint& endpoint) const
  {
    using Tag = hana::tag_of_t<Endpoint>;
    using Impl = endpoint_send_close_impl<Tag>;

    static_assert(
      nbdl::Endpoint<Tag>::value
    , "nbdl::endpoint_send_close(endpoint, message) "
      "requires endpoint to be a nbdl::Endpoint"
    );

    Impl::apply(endpoint);
  };

  template <typename Tag, bool condition>
  struct endpoint_send_close_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    template <typename Endpoint>
    static void apply(Endpoint& endpoint)
    {
      endpoint.send_close();
    }
  };

  template <typename Tag>
  struct endpoint_send_close_impl<Tag, hana::when<EndpointPtr<Tag>::value>>
  {
    template <typename Endpoint>
    static void apply(Endpoint& endpoint)
    {
      endpoint_send_close(*endpoint);
    }
  };
}

#endif
