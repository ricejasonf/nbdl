//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_UPSTREAM_MESSAGE_HPP
#define NBDL_CONCEPT_UPSTREAM_MESSAGE_HPP

#include<nbdl/fwd/concept/UpstreamMessage.hpp>

#include<nbdl/message.hpp>

#include<boost/hana/integral_constant.hpp>
#include<utility>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename T>
  struct UpstreamMessage<T>
    : hana::bool_<message::is_upstream<std::decay_t<T>>>
  { };
}

#endif
