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
  template <typename Delta, typename BindableSequence>
  constexpr bool apply_delta_fn::operator()(Delta const& d, BindableSequence&& e) const
  {
    using Tag = hana::tag_of_t<Delta>;
    using Impl = apply_delta_impl<Tag>;

    static_assert(nbdl::Delta<Delta>::value
      , "nbdl::apply_delta(delta, x) delta must be a Delta");

    static_assert(nbdl::BindableSequence<BindableSequence>::value
      , "nbdl::apply_delta(delta, x) x must be a BindableSequence");

    return Impl::apply(d, std::forward<BindableSequence>(e));
  };

  template <typename Tag, bool condition>
  struct apply_delta_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
}

#endif
