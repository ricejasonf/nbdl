//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_CONTEXT_HPP
#define NBDL_CONCEPT_CONTEXT_HPP

namespace nbdl {
  // Encapsulate a store such that it is
  // a root node in its state graph.
  template <typename T>
  concept Context = requires (T t) {
    Store auto&& store = t.nbdl_get_context_alias_value();
  }
}

#endif
