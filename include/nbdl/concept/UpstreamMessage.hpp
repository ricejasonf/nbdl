//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_UPSTREAM_MESSAGE_HPP
#define NBDL_CONCEPT_UPSTREAM_MESSAGE_HPP

#include<nbdl/fwd/concept/UpstreamMessage.hpp>

#include<nbdl/fwd/message.hpp>

#include<boost/hana/at.hpp>
#include<boost/hana/integral_constant.hpp>
#include<type_traits>
#include<utility>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct UpstreamMessage<T, hana::when<!hana::Sequence<T>::value>>
  {
    static constexpr bool value = false;
  };

  template<typename T>
  struct UpstreamMessage<T, hana::when<hana::Sequence<T>::value>>
  {
    static constexpr bool value = std::is_same<
      std::decay_t<decltype(hana::at(std::declval<T>(), hana::int_c<0>))>,
      message::channel::upstream
    >::value;
  };
} // nbdl

#endif
