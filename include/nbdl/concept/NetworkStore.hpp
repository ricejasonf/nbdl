//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_NEWTORK_STORE_HPP
#define NBDL_CONCEPT_NEWTORK_STORE_HPP

#include <nbdl/concept/HasImpl.hpp>

namespace nbdl {
  template <typename T>
  struct apply_message_impl;

  template <typename T>
  struct apply_foreign_message_impl;

  template <typename T>
  concept NetworkStore = HasImpl<T, nbdl::apply_message_impl> ||
                         HasImpl<T, nbdl::apply_foreign_message_impl>;
}

#endif
