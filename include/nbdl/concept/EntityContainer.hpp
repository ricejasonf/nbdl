//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_ENTITY_CONTAINER_HPP
#define NBDL_CONCEPT_ENTITY_CONTAINER_HPP

#include <nbdl/concept/Container.hpp>
#include <nbdl/concept/Entity.hpp>

namespace nbdl {
  template <typename T>
  concept EntityContainer = nbdl::Container<T> &&
                            nbdl::Entity<typename std::decay_t<T>::value_type>;
}

#endif
