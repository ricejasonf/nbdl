//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENTITY_HPP
#define NBDL_ENTITY_HPP

#include <nbdl/concept/Entity.hpp>
#include <nbdl/entity_members.hpp>
#include <nbdl/macros/NBDL_ENTITY.hpp>

#include <boost/hana/fwd/equal.hpp>

// NBDL_ENTITY should be used to create an 'entity'

namespace boost { namespace hana
{
  // Entity equals is more for testing than anything.
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
}} // boost::hana

#endif
