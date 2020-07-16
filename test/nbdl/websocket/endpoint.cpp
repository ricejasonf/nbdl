//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <catch.hpp>
#include <nbdl/catch.hpp>
#include <nbdl/endpoint_handler.hpp>
#include <nbdl/endpoint_open.hpp>
#include <nbdl/endpoint_send_close.hpp>
#include <nbdl/endpoint_send_message.hpp>
#include <nbdl/hold.hpp>
#include <nbdl/hold_lazy.hpp>
#include <nbdl/run_async.hpp>
#include <nbdl/websocket/client_endpoint.hpp>
#include <nbdl/websocket/server_endpoint.hpp>
#include <nbdl_test/asio_helpers.hpp>
#include <queue>
#include <string>

#define LARGE_MESSAGE_SIZE std::size_t{64 * 1024}

namespace asio = boost::asio;
namespace hana = boost::hana;
using tcp = boost::asio::ip::tcp;

namespace
{
  enum struct event_kind
  {
    READY
  , MESSAGE
  , CLOSE
  , BAD_REQUEST
  , ERROR
  };

  using test_events = std::vector<std::string>;
  constexpr auto make_test_event = [](event_kind kind, std::string str)
  {
    return std::string() + std::to_string(static_cast<int>(kind)) + " - " + str;
  };

  constexpr auto save_test_message = [](auto& events)
  {
    return [&events](event_kind kind)
    {
      // This kind of metaprogramming works but is very slow to compile
      return hana::overload_linearly(
        [&, kind](std::string const& value)
        {
          events.push_back(make_test_event(kind, value));
        }
      , [&, kind](nbdl::auth_token_t const& token)
        {
          events.push_back(make_test_event(kind, token.value));
        }
      , [&, kind](auto&& ...)
        {
          events.push_back(make_test_event(kind, std::string{}));
        }
      );
    };
  };

  constexpr auto save_and_close = [](auto save_message, auto&resolver)
  {
    return [save_message, &resolver](auto&& ...)
    {
      save_message();
      resolver.resolve();
    };
  };


  struct results_t
  {
    test_events server_events;
    test_events client_events;
  };

  template <typename MakeServerEndpoint, typename MakeClientEndpoint>
  inline results_t run_test(asio::io_service& io
                     , MakeServerEndpoint& run_server_endpoint
                     , MakeClientEndpoint& run_client_endpoint)
  {
    test_events client_events{};
    test_events server_events{};
    auto save_client_message = save_test_message(client_events);
    auto save_server_message = save_test_message(server_events);

    run_server_endpoint(save_server_message);
    run_client_endpoint(save_client_message);

    io.run();

    return results_t{server_events, client_events};
  }
}

TEST_CASE("Integrate server and client endpoint.", "[websocket][server_endpoint][client_endpoint]")
{
  asio::io_service io;
  namespace event = nbdl::websocket::event;

  // for sending
  std::vector<std::string> server_to_client_messages{};
  std::vector<std::string> client_to_server_messages{};

  auto run_server_endpoint = [&](auto save_event)
  {
    nbdl::run_async(hana::make_tuple(
      nbdl::hold(tcp::socket(io))
    , nbdl_test::accept()
    , nbdl::hold_lazy(
        [&, save_event](auto& resolver, tcp::socket& socket)
        {
          return nbdl::endpoint_open(
            nbdl::websocket::server_endpoint{std::move(socket)}
          , std::queue<std::string>{}
          , nbdl::endpoint_handler(
              event::on_ready = [&](auto& self, nbdl::auth_token_t const& token)
              {
                save_event(event_kind::READY)(token.value);
                for (auto& msg : server_to_client_messages)
                {
                  nbdl::endpoint_send_message(self, msg);
                }
                self.start_messaging();
              }
            , event::on_message = [&](auto& self, auto const& message)
              {
                save_event(event_kind::MESSAGE)(message);
                if (message == "close")
                {
                  nbdl::endpoint_send_close(self);
                }
              }
            , event::on_close             = save_and_close(save_event(event_kind::CLOSE), resolver)
            , event::on_bad_request       = save_and_close(save_event(event_kind::BAD_REQUEST), resolver)
            , event::on<std::error_code> = save_and_close(save_event(event_kind::ERROR), resolver)
            )
          );
        }
      , nbdl::lazy_args<tcp::socket&>
      )
    , nbdl::catch_([](auto) { CHECK(false); })
    ));
  };

  auto run_client_endpoint = [&](auto save_event)
  {
    nbdl::run_async(hana::make_tuple(
      nbdl::hold(tcp::socket(io))
    , nbdl_test::connect()
    , nbdl::hold_lazy(
        [&, save_event](auto& resolver, tcp::socket& socket)
        {
          return nbdl::endpoint_open(
            nbdl::websocket::client_endpoint{socket, {"Ima_auth_token"}, {}}
          , std::queue<std::string>{}
          , nbdl::endpoint_handler(
              event::on_ready = [&](auto& self)
              {
                save_event(event_kind::READY)();
                for (auto& msg : client_to_server_messages)
                {
                  nbdl::endpoint_send_message(self, msg);
                }
                self.start_messaging();
              }
            , event::on_message = [&](auto& self, auto const& message)
              {
                save_event(event_kind::MESSAGE)(message);
                if (message == "close")
                {
                  nbdl::endpoint_send_close(self);
                }
              }
            , event::on_close             = save_and_close(save_event(event_kind::CLOSE), resolver)
            , event::on_bad_request       = save_and_close(save_event(event_kind::BAD_REQUEST), resolver)
            , event::on<std::error_code> = save_and_close(save_event(event_kind::ERROR), resolver)
            )
          );
        }
      , nbdl::lazy_args<tcp::socket&>
      )
    , nbdl::catch_([](auto) { CHECK(false); })
    ));
  };

  server_to_client_messages = {
    "Hello, client! 1"
  , "Hello, client! 2"
  , "Hello, client! 3"
  , std::string(LARGE_MESSAGE_SIZE, 's')
  , "close"
  };

  client_to_server_messages = {
    "Hello, server! 1"
  , "Hello, server! 2"
  , "Hello, server! 3"
  , std::string(LARGE_MESSAGE_SIZE, 'c')
  , "close"
  };

  auto [server_events, client_events] = run_test(io, run_server_endpoint, run_client_endpoint);

  test_events expected_server_events{
    make_test_event(event_kind::READY   , std::string("Ima_auth_token"))
  , make_test_event(event_kind::MESSAGE , std::string("Hello, server! 1"))
  , make_test_event(event_kind::MESSAGE , std::string("Hello, server! 2"))
  , make_test_event(event_kind::MESSAGE , std::string("Hello, server! 3"))
  , make_test_event(event_kind::MESSAGE , std::string(LARGE_MESSAGE_SIZE, 'c'))
  , make_test_event(event_kind::MESSAGE , std::string("close"))
  , make_test_event(event_kind::CLOSE   , std::string())
  };

  CHECK(server_events == expected_server_events);

  test_events expected_client_events{
    make_test_event(event_kind::READY   , std::string())
  , make_test_event(event_kind::MESSAGE , std::string("Hello, client! 1"))
  , make_test_event(event_kind::MESSAGE , std::string("Hello, client! 2"))
  , make_test_event(event_kind::MESSAGE , std::string("Hello, client! 3"))
  , make_test_event(event_kind::MESSAGE , std::string(LARGE_MESSAGE_SIZE, 's'))
  , make_test_event(event_kind::MESSAGE , std::string("close"))
  , make_test_event(event_kind::CLOSE   , std::string())
  };

  CHECK(client_events == expected_client_events);
}
