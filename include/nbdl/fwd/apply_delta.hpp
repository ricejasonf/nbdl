//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_APPLY_DELTA_HPP
#define NBDL_FWD_APPLY_DELTA_HPP

#include <nbdl/concept/Delta.hpp>
#include <nbdl/concept/BindableSequence.hpp>

namespace nbdl {
  namespace hana = boost::hana;

  struct apply_delta_fn {
    template <Delta Delta, BindableSequence BindableSequence>
    constexpr bool operator()(Delta const&, BindableSequence&&) const;
  };

  constexpr apply_delta_fn apply_delta{};
}

#endif
