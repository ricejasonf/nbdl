//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <array>
#include <emscripten.h>
#include <nbdl/catch.hpp>
#include <nbdl/endpoint_handler.hpp>
#include <nbdl/hold.hpp>
#include <nbdl/hold_lazy.hpp>
#include <nbdl/run_async.hpp>
#include <nbdl/websocket_api/client_endpoint.hpp>
#include <nbdl/websocket_api/json_payload.hpp>
#include <string>
#include <string_view>
#include <queue>

namespace hana = boost::hana;
using namespace std::literals;
using nbdl::websocket_api::json_payload;

void block() { }

// Client endpoint send/receive messages to test ws server

namespace
{
  void init_server()
  {
    EM_ASM({
      var server = new window.WebSocket.Server({ port: 8080 });
      server.on('connection', function connection(ws)
      {
        ws.on('message', function message(data)
        {
          var val = JSON.parse(data);
          ws.send(JSON.stringify(val.toUpperCase()));
          if (val == 'close')
          {
            ws.close();
          }
        });
      });
    });
  }

  std::string to_string(json_payload const& p)
  {
    // A better solution would be to make a
    // std::string factory function accessible
    // from javascript land to prevent the
    // superfluous copy
    int ptr = EM_ASM_INT(
      {
        var str = Module.NBDL_DETAIL_JS_GET($0);
        var ptr = Module._malloc(str.length + 1);
        if (!ptr)
        {
          throw 'Memory allocation failed.';
        }
        Module.stringToAscii(str, ptr);
        return ptr;
      }
    , p.handle()
    );
    std::string str{reinterpret_cast<char*>(ptr)};
    EM_ASM_(
      {
        Module._free($0);
      }
    , ptr
    );
    return str;
  }

  void set_payload(json_payload& p, std::string_view const& str)
  {
    EM_ASM_(
      {
        Module.NBDL_DETAIL_JS_SET($0, Pointer_stringify($1, $2));
      }
    , p.handle()
    , str.data()
    , str.size()
    );
  }
}

int main()
{
  namespace event = nbdl::endpoint_event;

  init_server();

  nbdl::run_async(hana::make_tuple(
    nbdl::hold(std::vector<std::string>{})
  , nbdl::hold_lazy([&](auto& resolver, auto& received_messages)
    {
      return nbdl::endpoint_open(
        nbdl::websocket_api::client_endpoint{"ws://127.0.0.1:8080", {"Ima_auth_token"}, {}}
      , std::queue<nbdl::websocket_api::json_payload>{}
      , nbdl::endpoint_handler(
          hana::make_pair(event::ready, [&](auto& self)
          {
            std::array<std::string_view, 6> outgoing_messages{{
              "message #1"sv
            , "message #2"sv
            , "message #3"sv
            , "message #4"sv
            , "Hello, world!"sv
            , "close"sv
            }};

            json_payload payload{};
            for (auto const& x : outgoing_messages)
            {
              set_payload(payload, x);
              self.send_message(payload);
            }
          })
        , hana::make_pair(event::message, [&](auto&, auto const& payload)
          {
            received_messages.push_back(to_string(payload));
          })
        , hana::make_pair(event::close, [&](auto&, auto&&...)
          {
            if (received_messages == std::vector<std::string>{{
              "MESSAGE #1"s
            , "MESSAGE #2"s
            , "MESSAGE #3"s
            , "MESSAGE #4"s
            , "HELLO, WORLD!"s
            , "CLOSE"s
            }})
            {
              resolver.resolve();
            }
            else
            {
              resolver.reject(1);
            }
          })
        )
      );
    }, nbdl::lazy_args<std::vector<std::string>&>)
  , []
    {
      emscripten_force_exit(0);
    }
  , nbdl::catch_([](auto)
    {
      emscripten_force_exit(1);
    })
  ));

  emscripten_exit_with_live_runtime();
}
