//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONCEPT_DELTA_HPP
#define NBDL_CONCEPT_DELTA_HPP

#include <nbdl/concept/HasImpl.hpp>

namespace nbdl {
  namespace hana = boost::hana;

  template <typename Tag>
  struct apply_delta_impl : hana::default_ {
    static constexpr auto apply(...) = delete;
  };

  template <typename Tag>
  struct make_delta_impl : hana::default_ {
    static constexpr auto apply(...) = delete;
  };

  template <typename T>
  concept Delta = HasImpl<T, nbdl::make_delta_impl> &&
                  HasImpl<T, nbdl::apply_delta_impl>;
}

#endif
