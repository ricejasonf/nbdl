//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_DOWNSTREAM_MESSAGE_HPP
#define NBDL_CONCEPT_DOWNSTREAM_MESSAGE_HPP

#include<nbdl/fwd/concept/DownstreamMessage.hpp>

#include<nbdl/message.hpp>

#include<boost/hana/core/when.hpp>
#include<type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct DownstreamMessage<T, hana::when<true>>
  {
    static constexpr bool value = false;
  };

  template<typename T>
  struct DownstreamMessage<T, hana::when<hana::Sequence<T>::value>>
  {
    static constexpr bool value = std::is_same<
      decltype(message::get_channel(std::declval<T>())),
      message::channel::downstream
    >::value;
  };
} // nbdl

#endif
