//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_GET_HPP
#define NBDL_GET_HPP

#include<nbdl/concept/Store.hpp>
#include<nbdl/fwd/get.hpp>

#include<utility>

namespace nbdl
{
  template<typename Store, typename Path>
  constexpr decltype(auto) get_fn::operator()(Store&& s, Path&& p) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = get_impl<Tag>;

    static_assert(nbdl::Store<Store>::value,
      "nbdl::get(store, path) requires 'store' to be a Store");

    return Impl::apply(std::forward<Store>(s), std::forward<Path>(p));
  };

  template<typename Tag, bool condition>
  struct get_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };
} // nbdl

#endif
