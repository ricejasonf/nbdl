//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MATCH_HPP
#define NBDL_MATCH_HPP

#include <nbdl/concept/Store.hpp>
#include <nbdl/fwd/match.hpp>
#include <nbdl/get.hpp>

#include <boost/hana/core/default.hpp>
#include <boost/hana/functional/overload_linearly.hpp>
#include <utility>

namespace nbdl
{
  template<typename Store, typename Key, typename ...Fns>
  constexpr void match_fn::operator()(Store&& s, Key&& k, Fns&&... fns) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = match_impl<Tag>;

    static_assert(
      nbdl::Store<Store>::value
    , "nbdl::match(store, key) requires 'store' to be a Store"
    );

    Impl::apply(
      std::forward<Store>(s)
    , std::forward<Key>(k)
    , hana::overload_linearly(std::forward<Fns>(fns)...)
    );
  };

  template<typename Tag, bool condition>
  struct match_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  // Use nbdl::get if it is specialized.
  template<typename Tag>
  struct match_impl<Tag, hana::when<!hana::is_default<nbdl::get_impl<Tag>>::value>>
  {
    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      std::forward<Fn>(fn)(
        nbdl::get(std::forward<Store>(s), std::forward<Key>(k))
      );
    }
  };
} // nbdl

#endif
