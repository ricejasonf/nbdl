//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TAGS_HPP
#define NBDL_TAGS_HPP

#include <cstddef>

namespace nbdl {
  // Tags used for possible store values and keys

  //
  //  trigger_read  - nbdl::context and others can trigger a read action
  //                  when a contained store matches this tag.
  //
  struct trigger_read { };

  //
  //  not_found     - Used to indicate that a resource does not exist
  //                  as the result of a read action
  //
  struct not_found { };

  //
  //  not_in_set    - AssociativeContainers can yield this tag when a key
  //                  is not in the set and not read action is desired.
  //
  struct not_in_set { };

  //
  //  unresolved    - A placeholder tag indicating that the final value is
  //                  pending the result of a read action.
  //                  (Also the default for nbdl::variant)
  //
  struct unresolved { };

  // Provide a key to access elements of
  // tuple-likes and sequences by their index.
  // (ie similar to std::get<i>)
  template <size_t i> struct index_t { };
  template <size_t i>
  inline constexpr index_t<i> index;


  // Provide a key to access elements of
  // tuple-likes and sequences by their type.
  // (ie similar to std::get<T>)
  template <typename T> struct type_t { };
  template <typename T>
  inline constexpr type_t<T> type;
}

#endif
