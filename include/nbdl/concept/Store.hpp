//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_STORE_HPP
#define NBDL_CONCEPT_STORE_HPP

#include <nbdl/concept/HasImpl.hpp>

namespace nbdl {
  template <typename Tag>
  struct match_impl : hana::default_ {
    static constexpr auto apply(...) = delete;
  };

  template <typename T>
  concept Store = HasImpl<T, nbdl::match_impl>;
}

#endif
