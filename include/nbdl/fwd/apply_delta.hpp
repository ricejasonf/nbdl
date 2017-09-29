//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_APPLY_DELTA_HPP
#define NBDL_FWD_APPLY_DELTA_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct apply_delta_impl : apply_delta_impl<T, hana::when<true>> { };

  struct apply_delta_fn
  {
    template <typename Delta, typename BindableSequence>
    constexpr bool operator()(Delta const&, BindableSequence&&) const;
  };

  constexpr apply_delta_fn apply_delta{};
}

#endif
