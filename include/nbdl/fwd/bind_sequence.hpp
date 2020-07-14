//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_BIND_SEQUENCE_HPP
#define NBDL_FWD_BIND_SEQUENCE_HPP

#include <nbdl/concept/BindableSequence.hpp>

namespace nbdl
{
  struct bind_sequence_fn
  {
    template<BindableSequence BindableSequence, typename BindFn>
    constexpr auto operator()(BindableSequence&&, BindFn&&) const;
  };

  constexpr bind_sequence_fn bind_sequence{};
}

#endif
