//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBSOCKET_API_DETAIL_EVENT_RECEIVER_HPP
#define NBDL_WEBSOCKET_API_DETAIL_EVENT_RECEIVER_HPP

#include <nbdl/endpoint_handler.hpp>

#include <boost/hana/pair.hpp>
#include <boost/hana/string.hpp>
#include <dyno.hpp>

namespace nbdl::websocket_api::detail
{
  namespace event = nbdl::endpoint_event;
  namespace hana = boost::hana;

  constexpr auto value_handle_s = BOOST_HANA_STRING("value_handle");

  struct EventReceiver : decltype(dyno::requires(
    dyno::CopyConstructible{}
  , hana::make_pair(event::ready,   dyno::function<void(dyno::T&)>)
  , hana::make_pair(event::message, dyno::function<void(dyno::T&)>)
  , hana::make_pair(event::close,   dyno::function<void(dyno::T&)>)
  , hana::make_pair(value_handle_s, dyno::function< int(dyno::T const&)>)
  )) { };

  template <typename Endpoint>
  struct event_receiver_impl
  {
    void ready()    { endpoint.trigger(event::ready_t{}); }
    void message()  { endpoint.trigger(event::message_t{}); }
    void close()    { endpoint.trigger(event::close_t{}); }
    int value_handle() const { return endpoint.event_data.handle(); }

    Endpoint& endpoint;
  };
}

template <typename T>
constexpr auto ::dyno::default_concept_map<nbdl::websocket_api::detail::EventReceiver, T> =
  dyno::make_concept_map(
    boost::hana::make_pair(
      nbdl::endpoint_event::ready
    , [](T& self) { self.ready(); }
    )
  , boost::hana::make_pair(
      nbdl::endpoint_event::message
    , [](T& self) { self.message(); }
    )
  , boost::hana::make_pair(
      nbdl::endpoint_event::close
    , [](T& self) { self.close(); }
    )
  , boost::hana::make_pair(
      nbdl::websocket_api::detail::value_handle_s
    , [](T const& self) -> int { return self.value_handle(); }
    )
  );

#endif
