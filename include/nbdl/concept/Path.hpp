//
// Copyright Jason Rice 2020
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_PATH_HPP
#define NBDL_CONCEPT_PATH_HPP

#include <boost/hana/concept/sequence.hpp>
#include <boost/hana/is_empty.hpp>
#include <type_traits>

namespace nbdl {
  namespace hana = boost::hana;

  template <typename T>
  concept Path = hana::Sequence<T>::value;

  template <typename T>
  concept EmptyPath = Path<T> &&
          decltype(hana::is_empty(std::declval<T>()))::value;
}

#endif
