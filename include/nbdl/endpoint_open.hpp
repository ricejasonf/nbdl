//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_ENDPOINT_OPEN_HPP
#define NBDL_MAKE_ENDPOINT_OPEN_HPP

#include <nbdl/concept/Endpoint.hpp>
#include <nbdl/fwd/endpoint_open.hpp>

namespace nbdl
{
  template <typename Endpoint, typename SendQueue, typename Handler>
  constexpr auto endpoint_open_fn::operator()(Endpoint&& endpoint
                                            , SendQueue&& send_queue
                                            , Handler&& handler) const
  {
    using Tag = hana::tag_of_t<Endpoint>;
    using Impl = endpoint_open_impl<Tag>;

    static_assert(
      nbdl::Endpoint<Tag>::value
    , "nbdl::endpoint_open(endpoint, send_queue, handler) "
      "requires endpoint to be a nbdl::Endpoint"
    );

    return Impl::apply(
      std::forward<Endpoint>(endpoint)
    , std::forward<SendQueue>(send_queue)
    , std::forward<Handler>(handler)
    );
  };

  template <typename Tag, bool condition>
  struct endpoint_open_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr void apply(...) = delete;
  };
}

#endif
