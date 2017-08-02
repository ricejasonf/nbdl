//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENDPOINT_OPEN_HPP
#define NBDL_ENDPOINT_OPEN_HPP

#include <nbdl/concept/Endpoint.hpp>
#include <nbdl/concept/EndpointInfo.hpp>
#include <nbdl/fwd/endpoint_open.hpp>

namespace nbdl
{
  template <typename EndpointInfo, typename SendQueue, typename Handler>
  auto endpoint_open_fn::operator()(EndpointInfo&& info
                                            , SendQueue&& send_queue
                                            , Handler&& handler) const
  {
    using Tag = hana::tag_of_t<EndpointInfo>;
    using Impl = endpoint_open_impl<Tag>;

    static_assert(
      nbdl::EndpointInfo<Tag>::value
    , "nbdl::endpoint_open(endpoint_info, send_queue, handler) "
      "requires endpoint_info to be a nbdl::EndpointInfo"
    );

    using Return = decltype(Impl::apply(
      std::forward<EndpointInfo>(info)
    , std::forward<SendQueue>(send_queue)
    , std::forward<Handler>(handler)
    ));

    static_assert(
      nbdl::Endpoint<Return>::value
    , "nbdl::endpoint_open must return a nbdl::Endpoint"
    );

    return Impl::apply(
      std::forward<EndpointInfo>(info)
    , std::forward<SendQueue>(send_queue)
    , std::forward<Handler>(handler)
    );
  };

  template <typename Tag, bool condition>
  struct endpoint_open_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static auto apply(...) = delete;
  };
}

#endif
