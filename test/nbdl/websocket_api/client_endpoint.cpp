//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <emscripten.h>
#include <nbdl/catch.hpp>
#include <nbdl/endpoint_handler.hpp>
#include <nbdl/hold.hpp>
#include <nbdl/hold_lazy.hpp>
#include <nbdl/run_async.hpp>
#include <nbdl/websocket_api/client_endpoint.hpp>
#include <nbdl/websocket_api/json_payload.hpp>
#include <queue>

namespace hana = boost::hana;

// Client endpoint close on connection failure

int main()
{
  namespace event = nbdl::endpoint_event;

  nbdl::run_async(hana::make_tuple(
    nbdl::hold_lazy([&](auto& resolver)
    {
      return nbdl::endpoint_open(
        nbdl::websocket_api::client_endpoint{"ws://127.0.0.1", {"Ima_auth_token"}, {}}
      , std::queue<nbdl::websocket_api::json_payload>{}
      , nbdl::endpoint_handler(
          hana::make_pair(event::ready, [&](auto&)
          {
            resolver.reject(hana::type_c<void>);
          })
        , hana::make_pair(event::message, [&](auto&, auto const&)
          {
            resolver.reject(hana::type_c<void>);
          })
        , hana::make_pair(event::close, [&](auto&, auto&&...)
          {
            // TODO close reason
            resolver.resolve();
          })
        )
      );
    })
  , [] { emscripten_cancel_main_loop(); }
  , nbdl::catch_([](auto)
    {
      emscripten_force_exit(1);
    })
  ));

  emscripten_exit_with_live_runtime();
}
