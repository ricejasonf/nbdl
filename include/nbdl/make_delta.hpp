//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_DELTA_HPP
#define NBDL_MAKE_DELTA_HPP

#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/Delta.hpp>
#include <nbdl/fwd/make_delta.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/type.hpp>

namespace nbdl
{
  template <typename T>
  template <typename BindableSequence>
  constexpr decltype(auto) make_delta_fn<T>::operator()(BindableSequence const& x1,
                                                        BindableSequence const& x2) const
  {
    using Tag = hana::tag_of_t<T>;
    using Impl = make_delta_impl<Tag>;

    using Return = decltype(Impl::apply(x1, x2));

    static_assert(nbdl::BindableSequence<BindableSequence>::value
      , "nbdl::make_delta<T>(x, y) x and y must satisfy BindableSequence");

    static_assert(
      nbdl::Delta<Return>::value
      , "nbdl::make_delta<T>(x, y) must return a Delta.");

    return Impl::apply(x1, x2);
  };

  template <typename Tag, bool condition>
  struct make_delta_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
