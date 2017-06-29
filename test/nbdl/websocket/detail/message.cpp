//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
  #include <iostream> // TODO
  #include <nbdl/tap.hpp>

#include <catch.hpp>
#include <nbdl/catch.hpp>
#include <nbdl/run_async.hpp>
#include <nbdl/websocket/detail/message_reader.hpp>
#include <nbdl/websocket/detail/message_sender.hpp>
#include <nbdl_test/asio_helpers.hpp>
#include <string>

namespace hana = boost::hana;

namespace
{
  /*
  using nbdl::websocket::detail::send_binary;
  using nbdl::websocket::detail::send_close;
  using nbdl::websocket::detail::send_ping;
  using nbdl::websocket::detail::send_pong;
  using nbdl::websocket::detail::send_text;
  */

  enum struct message_kind
  {
    DATA
  , PING
  , PONG
  , CLOSE
  , BAD_INPUT
  , ERROR
  };

  // string is the payload
  using test_message  = hana::pair<message_kind, std::string>;
  using test_messages = std::vector<test_message>;

  // client sends inputs
  // returns messages received by server
  template <typename SendStuff>
  inline test_messages run_test(SendStuff&& send_stuff)
  {
    using nbdl::websocket::detail::make_message_reader;
    using nbdl::websocket::detail::keep_reading;
    using nbdl::websocket::detail::message_sender;
    namespace read_event = nbdl::websocket::detail::read_event;

    test_messages messages{};

    auto save_test_message = [&messages](message_kind kind)
    {
      return [&messages, kind](auto, auto ctx)
      {
        std::string payload{};
        if (kind == message_kind::DATA || kind == message_kind::CLOSE)
        {
          payload = std::string(ctx.payload.size(), 0);
          std::copy(ctx.payload.begin(), ctx.payload.end(), payload.begin());
        }
        messages.push_back(hana::make_pair(kind, std::move(payload)));
      };
    };

    // TODO make a socket holder
    asio::io_service io;
    auto server_socket = hana::always(asio::ip::tcp::socket(io));
    auto client_socket = hana::always(asio::ip::tcp::socket(io));

    auto sender = message_sender<std::string>(client_socket());
    auto reader = make_message_reader(
      hana::type_c<std::string>
    , server_socket()
    , hana::make_map(
        hana::make_pair(read_event::message           , save_test_message(message_kind::DATA))
      , hana::make_pair(read_event::close             , save_test_message(message_kind::CLOSE))
      , hana::make_pair(read_event::ping              , save_test_message(message_kind::PING))
      , hana::make_pair(read_event::pong              , save_test_message(message_kind::PONG))
      , hana::make_pair(read_event::bad_input         , save_test_message(message_kind::BAD_INPUT))
      , hana::make_pair(hana::type_c<std::error_code> , save_test_message(message_kind::ERROR))
      )
    );

    nbdl::run_async(hana::make_tuple(
      std::ref(server_socket)
    , nbdl_test::accept()
    , keep_reading(reader)
    , nbdl::catch_([](auto) { CHECK(false); })
    ));

    nbdl::run_async(hana::make_tuple(
      std::ref(client_socket)
    , nbdl_test::connect()
    , [&](auto&& ...) -> auto& { return sender; }
    , std::forward<SendStuff>(send_stuff)
    , nbdl::catch_([](auto) { CHECK(false); })
    ));

    io.run();

    return messages;
  }

  constexpr auto send_partial_frame = [](std::string payload, unsigned char opcode)
  {
    return nbdl::promise([payload = std::move(payload), opcode](auto& resolver, auto& sender)
    {
      sender.set_opcode(opcode);
      sender.send_frame_impl(
        asio::buffer(payload)
      , [&](auto error_code, std::size_t)
        {
          if (error_code)
          {
            resolver.reject(error_code);
          }
          else
          {
            resolver.resolve(sender);
          }
        }
      );
    });
  };
}

TEST_CASE("Send and receive messages with interjecting control frames.", "[websocket]")
{
  auto input = hana::make_tuple(
    nbdl::promise([](auto& resolver, auto& sender)
    {
      sender.data.payload = "Hello, world!";
      nbdl::websocket::detail::send_text(resolver, sender);
    })
  , nbdl::promise([](auto& resolver, auto& sender)
    {
      sender.data.payload = "Hello, world, again!";
      nbdl::websocket::detail::send_text(resolver, sender);
    })
  , send_partial_frame("Hello, ", 0x1) // text
  , nbdl::promise([](auto& resolver, auto& sender)
    {
      sender.data.payload.clear();
      nbdl::websocket::detail::send_ping(resolver, sender);
    })
  , send_partial_frame("ping!", 1 << 7) // coninuation with fin bit
  , nbdl::promise([](auto& resolver, auto& sender)
    {
      sender.data.payload.clear();
      nbdl::websocket::detail::send_close(resolver, sender);
    })
  );

  test_messages result = run_test(input);

  test_messages expected{
    hana::make_pair(message_kind::DATA , std::string("Hello, world!"))
  , hana::make_pair(message_kind::DATA , std::string("Hello, world, again!"))
  , hana::make_pair(message_kind::PING , std::string())
  , hana::make_pair(message_kind::DATA , std::string("Hello, ping!"))
  , hana::make_pair(message_kind::CLOSE, std::string())
  };

  CHECK(result == expected);
}

TEST_CASE("Send and receive messages across multiple data frames.", "[websocket]")
{
  // TODO
}
