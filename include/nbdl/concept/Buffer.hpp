//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_BUFFER_HPP
#define NBDL_CONCEPT_BUFFER_HPP

#include <nbdl/concept/Container.hpp>

#include <type_traits>
#include <utility>

namespace nbdl {
  template<typename T>
  concept Buffer = nbdl::ContiguousByteContainer<T> &&
                   nbdl::FixedSizeContainer<T>;
}

#endif
