//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_BINDABLE_VARIANT_HPP
#define NBDL_CONCEPT_BINDABLE_VARIANT_HPP

#include <nbdl/concept/HasImpl.hpp>

namespace nbdl {
  template <typename Tag>
  struct bind_variant_impl : default_impl {
    static constexpr auto apply(...) = delete;
  };

  template <typename T>
  concept BindableVariant = HasImpl<T, nbdl::bind_variant_impl>;
}

#endif

