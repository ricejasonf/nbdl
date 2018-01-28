//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_EVENT_HPP
#define NBDL_WEBSOCKET_EVENT_HPP

#include <nbdl/endpoint_handler.hpp>

#include <boost/hana/type.hpp>

namespace nbdl::websocket::event
{
  namespace hana = boost::hana;
  using namespace nbdl::endpoint_event;

  struct bad_request_t { };

  constexpr hana::type<bad_request_t> bad_request{};
  constexpr auto on_bad_request = on<bad_request_t>;
}

#endif
