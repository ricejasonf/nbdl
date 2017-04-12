//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_GET_HPP
#define NBDL_GET_HPP

#include <nbdl/concept/Store.hpp>
#include <nbdl/fwd/get.hpp>

#include <boost/hana/at_key.hpp>
#include <boost/hana/concept/searchable.hpp>
#include <boost/hana/index_if.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/functional/compose.hpp>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  template<typename Store, typename Key>
  constexpr decltype(auto) get_fn::operator()(Store&& s, Key&& k) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = get_impl<Tag>;

    static_assert(nbdl::Store<Store>::value,
      "nbdl::get(store, path) requires 'store' to be a Store");

    return Impl::apply(std::forward<Store>(s), std::forward<Key>(k));
  };

  template<typename Tag, bool condition>
  struct get_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  template<typename Tag>
  struct get_impl<Tag, hana::when<hana::Searchable<Tag>::value>>
  {
    template <typename Store, typename Key>
    static constexpr decltype(auto) apply(Store&& s, Key&& k)
    {
      if constexpr(hana::Sequence<Store>::value)
      {
        using Pred = decltype(hana::compose(hana::equal.to(hana::typeid_(k)), hana::typeid_));
        using Index = decltype(hana::index_if(s, Pred{}).value());
        return hana::at(std::forward<Store>(s), Index{});
      }
      else
      {
        return hana::at_key(std::forward<Store>(s), std::forward<Key>(k));
      }
    }
  };
} // nbdl

#endif
