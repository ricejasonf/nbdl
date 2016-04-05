//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_CONCEPT_DOWNSTREAM_MESSAGE_HPP
#define NBDL_FWD_CONCEPT_DOWNSTREAM_MESSAGE_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl
{
  template<typename T, typename = void>
  struct DownstreamMessage : DownstreamMessage<T, boost::hana::when<true>> { };
} // nbdl

#endif
