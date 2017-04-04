//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENTITY_HPP
#define NBDL_ENTITY_HPP

#include <nbdl/concept/Entity.hpp>
#include <nbdl/concept/Store.hpp>
#include <nbdl/entity_members.hpp>
#include <nbdl/match.hpp>
#include <nbdl/macros/NBDL_ENTITY.hpp>

#include <boost/hana/fwd/equal.hpp>

// NBDL_ENTITY should be used to create an 'entity'

namespace nbdl
{
  // Store

  template<typename Tag>
  struct match_impl<Tag, hana::when<nbdl::Entity<Tag>::value>>
  {
    template <typename Store, typename Path, typename ...Fn>
    static constexpr void apply(Store&& s, Path&& p, Fn&&... fn)
    {
      using Key = typename decltype(hana::typeid_(hana::front(p)))::type;
      auto const& value = get_member<Key>(std::forward<Store>(s));
      if constexpr(nbdl::Store<decltype(value)>::value)
      {
        match(
          value
        , hana::drop_front(std::forward<Path>(p))
        , std::forward<Fn>(fn)...
        );
      }
      else
      {
        hana::overload_linearly(std::forward<Fn>(fn)...)(value);
      }
    }
  };

  template<typename Tag>
  struct apply_action_impl<Tag, hana::when<nbdl::Entity<Tag>::value>>
  {
    static constexpr auto apply(...)
    {
      // TODO Perhaps actions would be useful for updating elements.
      return hana::false_c;
    }
  };
}

namespace boost::hana
{
  // Comparable

  template <typename T>
  struct equal_impl<T, T, hana::when<nbdl::Entity<T>::value>>
  {
    template <typename E>
    static constexpr auto apply(E const& e1, E const& e2)
    {
      return hana::unpack(nbdl::entity_members<T>, [&](auto ...m)
      {
        return hana::and_(
          hana::equal(
            nbdl::get_member<decltype(m)>(e1),
            nbdl::get_member<decltype(m)>(e2)
          )...
        );
      });
    }
  };
}

#endif
