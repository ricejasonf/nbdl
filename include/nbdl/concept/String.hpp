//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_STRING_HPP
#define NBDL_CONCEPT_STRING_HPP

#include <nbdl/concept/DynamicBuffer.hpp>
#include <type_traits>

namespace nbdl {

  // `is_string` stipulates that the type
  // is used as a container of contiguous
  // bytes that are unicode characters
  // terminated by null
  template <typename T>
  inline constexpr bool is_string = false;

  template <typename T>
  concept String = nbdl::DynamicBuffer<T> && is_string<std::decay_t<T>>;
}

#endif
