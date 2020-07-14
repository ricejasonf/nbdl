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

namespace nbdl {
  template <typename T>
  template <BindableSequence X>
  constexpr auto make_delta_fn<T>::operator()(X&& x) const {
    using Tag = hana::tag_of_t<T>;
    using Impl = make_delta_impl<Tag>;

    using Return = decltype(Impl::apply(std::forward<X>(x)));


    static_assert(nbdl::Delta<Return>,
        "nbdl::make_delta_impl must return a Delta");

    return Impl::apply(std::forward<X>(x));
  };

  template <typename T>
  template <BindableSequence X, typename Y>
  constexpr auto make_delta_fn<T>::operator()(X&& x, Y&& y) const {
    using Tag = hana::tag_of_t<T>;
    using Impl = make_delta_impl<Tag>;

    using Return = decltype(Impl::apply(std::forward<X>(x), std::forward<Y>(y)));

    static_assert(
      decltype(hana::equal(hana::typeid_(x), hana::typeid_(y)))::value
    , "nbdl::make_delta<T>(x, y) x and y must must be the same type"
    );

    static_assert(nbdl::Delta<Return>,
        "nbdl::make_delta_impl must return a Delta");

    return Impl::apply(std::forward<X>(x), std::forward<Y>(y));
  };
}

#endif
