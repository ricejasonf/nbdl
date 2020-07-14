//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_DYNAMIC_BUFFER_HPP
#define NBDL_CONCEPT_DYNAMIC_BUFFER_HPP

#include <nbdl/concept/Container.hpp>

#include <type_traits>
#include <utility>

namespace nbdl {
  template<typename T>
  concept DynamicBuffer = ContiguousByteContainer<T> && requires (T) {
    // We are not concerned with whether we can resize it but that
    // it has a dynamic length so const qualifiers are okay here.
    std::remove_cvref_t<T>{}.resize(std::size_t{0}, char{0});
  };
}

#endif
