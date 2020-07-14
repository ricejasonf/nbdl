//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APPLY_DELTA_HPP
#define NBDL_APPLY_DELTA_HPP

#include <nbdl/fwd/apply_delta.hpp>

#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/Delta.hpp>

namespace nbdl
{
  template <Delta Delta, BindableSequence BindableSequence>
  constexpr bool apply_delta_fn::operator()(Delta const& d, BindableSequence&& e) const
  {
    using Tag = hana::tag_of_t<Delta>;
    using Impl = apply_delta_impl<Tag>;

    return Impl::apply(d, std::forward<BindableSequence>(e));
  };
}

#endif
