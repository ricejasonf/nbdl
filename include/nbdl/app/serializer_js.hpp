//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERIALIZER_JS_HPP
#define NBDL_SERIALIZER_JS_HPP

#include <nbdl/binder/js.hpp>
#include <nbdl/message_api.hpp>

#include <string>

namespace nbdl
{
  namespace hana = boost::hana;

  // serializer - An object to be held by an actor and
  //              has a simple serialize function returning
  //              a reference to its buffer
  template <typename Tag>
  struct serializer
  {
    using downstream_variant = typename
      message_api<Tag>::downstream_variant;

    nbdl::js::val val = {};

    serializer(serializer const&) = delete;

    // converts to the downstream variant type
    // and serializes
    template <typename Message>
    nbdl::js::val& serialize(Message&& m) {
      using variant = downstream_variant;
      using nbdl::binder::js::bind_to;

      bind_to(val, variant(std::forward<Message>(m)));
      return val;
    }
  };

  // deserializer - A nullary function that returns a promise
  //                to handle deserialization and validation
  template <typename Tag>
  struct deserializer_fn
  {
    using upstream_variant = typename
      message_api<Tag>::upstream_variant;
    using nbdl::binder::js::bind_from;

    auto operator()() const {
      return promise([](auto& resolve, nbdl::js::val const& js_val) {
        using full_duplex::make_error;
        api::upstream_variant var;

        // populates var with data from the buffer in js land
        bind_from(js_val, var);

        nbdl::match(var, hana::overload_linearly(
          [&resolve](nbdl::unresolved) {
            // this should not happen unless we get
            // garbage input or something
            resolve(make_error("Unknown Error"));
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
