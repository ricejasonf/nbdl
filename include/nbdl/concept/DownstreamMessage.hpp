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

#include<boost/hana/integral_constant.hpp>
#include<type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct DownstreamMessage<T>
    : hana::bool_<message::is_downstream<std::decay_t<T>>>
  { };
}

#endif
