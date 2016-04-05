//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_DOWNSTREAM_MESSAGE_HPP
#define NBDL_CONCEPT_DOWNSTREAM_MESSAGE_HPP

#include<nbdl/fwd/concept/DownstreamMessage.hpp>

#include<nbdl/fwd/message.hpp>

#include<boost/hana/at.hpp>
#include<boost/hana/core/when.hpp>
#include<boost/hana/integral_constant.hpp>
#include<type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct DownstreamMessage<T, hana::when<!hana::Sequence<T>::value>>
  {
    static constexpr bool value = false;
  };

  template<typename T>
  struct DownstreamMessage<T, hana::when<hana::Sequence<T>::value>>
  {
    static constexpr bool value = std::is_same<
      decltype(hana::at(std::declval<T>(), hana::int_c<0>)),
      message::channel::downstream
    >::value;
  };
} // nbdl

#endif
