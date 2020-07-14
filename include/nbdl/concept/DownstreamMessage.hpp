//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_DOWNSTREAM_MESSAGE_HPP
#define NBDL_CONCEPT_DOWNSTREAM_MESSAGE_HPP

#include <nbdl/message.hpp>

#include <utility>

namespace nbdl {
  template<typename T>
  concept DownstreamMessage = message::is_downstream<std::decay_t<T>>;
}

#endif
