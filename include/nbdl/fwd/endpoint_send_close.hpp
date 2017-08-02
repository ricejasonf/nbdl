//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_ENDPOINT_SEND_CLOSE_HPP
#define NBDL_FWD_ENDPOINT_SEND_CLOSE_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct endpoint_send_close_impl : endpoint_send_close_impl<T, hana::when<true>> { };

  struct endpoint_send_close_fn
  {
    template <typename Endpoint>
    void operator()(Endpoint&) const;
  };

  constexpr endpoint_send_close_fn endpoint_send_close{};
}

#endif
