//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPTS_UPSTREAM_MESSAGE_HPP
#define NBDL_CONCEPTS_UPSTREAM_MESSAGE_HPP

#include<Message.hpp>// FIXME

#include<boost/hana/core/when.hpp>
#include<type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct UpstreamMessage
  {
    static constexpr bool value = false;
  };

  template<typename T>
  struct UpstreamMessage<T, hana::when<hana::Sequence<T>::value>>
  {
    static constexpr bool value = std::is_same<
      decltype(message::getChannel(std::declval<T>())),
      message::channel::Upstream
    >::value;
  };
} // nbdl

#endif
