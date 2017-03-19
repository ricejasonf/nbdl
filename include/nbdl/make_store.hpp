//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MAKE_STORE_HPP
#define NBDL_MAKE_STORE_HPP

#include <nbdl/fwd/make_store.hpp>

#include <nbdl/concept/Store.hpp>

#include <boost/hana/core/default.hpp>
#include <boost/hana/core/make.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <type_traits>

namespace nbdl
{
  template <typename T>
  template <typename PathType, typename EntityType>
  constexpr auto make_store_fn<T>::operator()(PathType const&, EntityType const&) const
  {
    using Tag = hana::tag_of_t<T>;
    using Impl = make_store_impl<Tag>;
    using Return = decltype(Impl::apply(PathType{}, EntityType{}));

    static_assert(
      nbdl::Store<Return>::value
      && std::is_default_constructible<Return>::value
    , "nbdl::make_store<T>(path_type, entity_type) must return default constructible Store."
    );
    return Return{};
  };

  template <typename Tag, bool condition>
  struct make_store_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...)
    {
      return hana::make<Tag>();
    }
  };
} // nbdl

#endif
