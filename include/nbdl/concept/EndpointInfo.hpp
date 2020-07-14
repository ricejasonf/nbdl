//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_ENDPOINT_INFO_HPP
#define NBDL_CONCEPT_ENDPOINT_INFO_HPP

#include <nbdl/concept/HasImpl.hpp>

namespace nbdl {
  template <typename Tag>
  struct endpoint_open_impl : default_impl {
    static auto apply(...) = delete;
  };
  
  template <typename T>
  concept EndpointInfo = HasImpl<T, nbdl::endpoint_open_impl>;
}

#endif
