//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at // http://www.boost.org/LICENSE_1_0.txt)
//

#if EMSCRIPTEN
#include <emscripten.h>
#endif

#include <array>
#include <boost/hana.hpp>
#include <full_duplex.hpp>
#include <nbdl.hpp>
#include <nbdl/app/client.hpp>
#include <nbdl/app/server.hpp>
#include <nbdl_test/consumer.hpp>
#include <queue>
#include <string>
#include <string_view>


namespace hana = boost::hana;
using namespace std::literals;

// TODO suport native client
// (the only difference in platforms should be the dummy server)
// (it would be nice to make the server a separate process)

namespace
{
#ifdef EMSCRIPTEN
  void force_exit(int code) {
    EM_ASM({ process.exit($0) }, code);
  }
#elif // NOT EMSCRIPTEN
  void force_exit(int code) {
    std::exit(code);
  }
#endif // NOT EMSCRIPTEN

  auto get_data_model = [] {
    using namespace nbdl_def;
    return AccessPoint(
      PathKey<hana::string<'f', 'o', 'o'>>,
      Entity<std::string>,
      Actions(Create())
    );
  };

  struct server_context {
    static auto make_def() {
      using namespace nbdl_def;
      return Context(
        Producers(Producer(
          Type<nbdl::echo_producer>,
          Name("echo"),
          get_data_model()
        )),
        Consumers(Consumer(
          Type<nbdl::app::server>,
          Name("server")
        ))
      );
    }
  };

  struct client_context {
    static auto make_def() {
      using namespace nbdl_def;
      return Context(
        Producers(Producer(
          Type<nbdl::app::client>,
          Name("client"),
          get_data_model()
        )),
        Consumers(Consumer(
          Type<nbdl_test::consumer>,
          Name("test_consumer")
        ))
      );
    }
  };
}

int main()
{
  namespace event = full_duplex::event;
  using full_duplex::endpoint;
  using full_duplex::endpoint_compose;
  using full_duplex::promise;
  using full_duplex::tap;

  std::vector<std::string> received_messages{};

  auto check = [&] {
    if (received_messages == std::vector<std::string>{{
      "message #1"s
    , "message #2"s
    , "message #3"s
    , "message #4"s
    , "Hello, world!"s
    , "close"s
    }}) {
      // PASS
      force_exit(0);
    }
    else {
      // FAIL
      force_exit(1);
    }
  };

  auto on_message = [&](auto msg) {
    bool is_close = (msg == "close"sv);
    received_messages.push_back(std::move(msg));
    if (is_close) {
      check();
    }
  };

  auto client_ctx = nbdl::make_context<client_context>(
    nbdl::actor("client", "ws://127.0.0.1:8080"),
    nbdl::actor("test_consumer", on_message)
  );

  auto server_ctx = nbdl::make_context<server_context>(
    nbdl::actor("server", nbdl::app::server{8080})
  );

  auto init = [&] {
    std::array<std::string_view, 6> outgoing_messages{{
      "message #1"sv
    , "message #2"sv
    , "message #3"sv
    , "message #4"sv
    , "Hello, world!"sv
    , "close"sv
    }};


    for (auto const& x : outgoing_messages)
    {
      nbdl::apply_message(*client_ctx, nbdl::message::make_upstream_create(
        hana::make_tuple(BOOST_HANA_STRING("foo"))
      , nbdl::message::no_uid
      , std::string(x)
      ));
    }
  };

  init();

#if EMSCRIPTEN
  emscripten_exit_with_live_runtime();
#endif
}
