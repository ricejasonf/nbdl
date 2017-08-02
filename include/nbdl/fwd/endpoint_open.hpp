//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ENDPOINT_OPEN_HPP
#define NBDL_FWD_ENDPOINT_OPEN_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct endpoint_open_impl : endpoint_open_impl<T, hana::when<true>> { };

  struct endpoint_open_fn
  {
    template <typename EndpointInfo, typename SendQueue, typename EndpointHandler>
    auto operator()(EndpointInfo&&, SendQueue&&, EndpointHandler&&) const;
  };

  constexpr endpoint_open_fn endpoint_open{};
}

#endif
