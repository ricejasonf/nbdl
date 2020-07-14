//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_CONTAINER_HPP
#define NBDL_CONCEPT_CONTAINER_HPP

#include<mpdef/list.hpp>

#include <array> // for std::tuple_size
#include <iterator>
#include <utility>

namespace nbdl {
  template <typename T>
  concept Container = requires (T t) {
    std::size(t);
    std::begin(t);
    std::end(t);
    typename std::decay_t<T>::value_type;
  };

  template <typename T>
  concept ContiguousContainer = Container<T> && requires (T t) {
    std::data(t);
  };

  template <typename T>
  concept ContiguousByteContainer = ContiguousContainer<T> &&
     (sizeof(typename std::decay_t<T>::value_type) == 1);

  template <typename T>
  concept FixedSizeContainer = Container<T> && requires (T t) {
    std::tuple_size<std::decay_t<T>>::value;
  };
}

#endif
