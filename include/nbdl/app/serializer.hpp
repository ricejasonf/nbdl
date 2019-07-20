//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SERIALIZER_HPP
#define NBDL_SERIALIZER_HPP

#ifdef EMSCRIPTEN
#include <nbdl/serializer_js.hpp>
#else // NOT EMSCRIPTEN

#include <nbdl/binder/jsoncpp.hpp>
#include <nbdl/message_api.hpp>

#include <string>

//
// TODO: These concrete serializers currently use the jsoncpp and js depending
//       on the detected platform. Eventually I would like to replace this with
//       binary serialization that is the same on all platforms.
//       Further out still, this should be Conceptified to allow other
//       serialization types such as Protobuf et al.
//

namespace nbdl
{
  namespace hana = boost::hana;

  // serializer - An object to be held by an actor and
  //                      has a simple serialize function returning
  //                      a reference to its buffer
  template <typename Tag>
  struct serializer
  {
    using variant = typename message_api<Tag>::downstream_variant;

    std::string buffer = {};

    serializer(serializer const&) = delete;

    // converts to the downstream variant type
    // and serializes
    template <typename Message>
    std::string& serialize(Message&& m) {
      using nbdl::binder::jsoncpp::to_string;
      buffer = to_string(variant(std::forward<Message>(m)));
      return (buffer);
    }
  };

  // deserializer - A nullary function that returns a promise
  //                to handle deserialization and validation
  template <typename Tag>
  struct deserialize_fn
  {
    using variant = typename message_api<Tag>::upstream_variant;

    auto operator()() const {
      return promise([](auto& resolve, std::string const& msg_buf) {
        using full_duplex::make_error;
        variant var;

        try {
          nbdl::binder::jsoncpp::from_string(msg_buf, var);
        } catch(...) {
          resolve(make_error("JSONCPP Parse Error"));
        }

        nbdl::match(std::move(var), hana::overload_linearly(
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

  template <typename Tag>
  constexpr deserialize_fn<Tag> deserialize{};
}

#endif // NOT EMSCRIPTEN
#endif
