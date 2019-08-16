//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APP_SERIALIZER_JS_HPP
#define NBDL_APP_SERIALIZER_JS_HPP

#include <nbdl/binder/js.hpp>
#include <string>

namespace nbdl::app
{
  namespace hana = boost::hana;
  using full_duplex::promise;

  // serializer - An object to be held by an actor and
  //              has a simple serialize function returning
  //              a reference to its buffer
  template <typename Tag, typename Variant>
  struct serializer
  {
    nbdl::js::val val = {};

    serializer(serializer const&) = delete;

    // converts to the variant type
    // and serializes
    template <typename Message>
    nbdl::js::val& serialize(Message&& m) {
      using nbdl::binder::js::bind_to;
      bind_to(val, Variant(std::forward<Message>(m)));
      return val;
    }
  };

  // deserializer - A nullary function that returns a promise
  //                to handle deserialization and validation
  template <typename Tag, typename Variant>
  struct deserializer_fn
  {
    auto operator()() const {
      using nbdl::binder::js::bind_from;
      return promise([](auto& resolve, nbdl::js::val const& js_val) {
        using full_duplex::make_error;
        Variant var;

        // populates var with data from the buffer in js land
        bind_from(js_val, var);

        nbdl::match(var, hana::overload_linearly(
          [&resolve](nbdl::unresolved) {
            // this should not happen unless we get
            // garbage input or something
            resolve(make_error("Deserialization failed"));
          },
          [&resolve](system_message sys_msg) {
            // something bad happened?
            resolve(make_error(std::move(sys_msg)));
          },
          [&resolve](auto&& msg) {
            resolve(std::forward<decltype(msg)>(msg));
          }));
      });
    }
  };
}

#endif
