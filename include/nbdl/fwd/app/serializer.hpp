//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_APP_SERIALIZER_HPP
#define NBDL_FWD_APP_SERIALIZER_HPP

#include <nbdl/message_api.hpp>

namespace nbdl::app
{
  // serializer - An object to be held by an actor and
  //                      has a simple serialize function returning
  //                      a reference to its buffer
  template <typename Tag, typename Variant>
  struct serializer;

  template <typename Tag>
  using serializer_downstream = serializer<
    Tag, typename message_api<Tag>::downstream_variant>;
  template <typename Tag>
  using serializer_upstream = serializer<
    Tag, typename message_api<Tag>::upstream_variant>;

  template <typename Tag, typename Variant>
  struct deserializer_fn;

  template <typename Tag>
  constexpr deserializer_fn<
      Tag, typename nbdl::message_api<Tag>::downstream_variant>
  deserializer_downstream{};

  template <typename Tag>
  constexpr deserializer_fn<
      Tag, typename nbdl::message_api<Tag>::upstream_variant>
  deserializer_upstream{};
}
#endif
