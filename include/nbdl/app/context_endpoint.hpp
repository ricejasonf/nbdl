//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APP_CONTEXT_ENDPOINT_HPP
#define NBDL_APP_CONTEXT_ENDPOINT_HPP

#include <full_duplex.hpp>
#include <full_duplex/future.hpp>
#include <nbdl.hpp>
#include <utility>

#if EMSCRIPTEN
#include <nbdl/js.hpp>
#define NBDL_APP_CONSOLE_LOG(x) emscripten::val::global("console").template call<void>("log", emscripten::val(x))
#else
#include <iostream> // for log_error
#define NBDL_APP_CONSOLE_LOG(x) std::cerr << '\n' << x << '\n'
#endif

namespace nbdl::app {
  namespace hana = boost::hana;
  using full_duplex::promise;
  using full_duplex::tap;
  using full_duplex::void_input;

  // Sends the message to the context
  // and resolves "void"
  constexpr auto apply_read = promise([](auto& resolve, auto&& msg) {
    // TODO perhaps get_state() should return a Store object
    //      that is the proxies the rest of the context
    nbdl::apply_message(resolve.get_state().context(),
                        std::forward<decltype(msg)>(msg));
    resolve(void_input);
  });

  constexpr auto log_error = hana::overload_linearly(
    [](nbdl::system_message const& err) {
      NBDL_APP_CONSOLE_LOG(err.value);
    },
    [](full_duplex::queue_full_t const&) {
      NBDL_APP_CONSOLE_LOG("QUEUE FULL");
    },
#if EMSCRIPTEN
    [](nbdl::js::val const& err) {
      NBDL_JS_TRANSFORM(err, function(err) {
        console.error(err);
        return err;
      });
    },
#else  /* NOT EMSCRIPTEN */
    [](boost::system::error_code const& err) {
      NBDL_APP_CONSOLE_LOG(err.message);
    },
#endif /* EMSCRIPTEN */
    [](auto const& err) {
      NBDL_APP_CONSOLE_LOG(err);
    });
}

#endif
