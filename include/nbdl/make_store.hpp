//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_STORE_HPP
#define NBDL_MAKE_STORE_HPP

#include<nbdl/fwd/make_store.hpp>

#include<nbdl/concept/Store.hpp>

namespace nbdl
{
  template <typename T>
  template <typename PathType>
  constexpr auto make_store_fn<T>::operator()(PathType const&) const
  {
    using Tag = hana::tag_of_t<T>;
    using Impl = make_store_impl<Tag>;
    using Return = decltype(Impl::apply(PathType{}));

    static_assert(
      nbdl::Store<Return>::value
      && std::is_default_constructible<Return>::value
      , "nbdl::make_store<T>(path_type) must return a Store (default constructible)");
    return Return{};
  };

  template <typename Tag, bool condition>
  struct make_store_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
