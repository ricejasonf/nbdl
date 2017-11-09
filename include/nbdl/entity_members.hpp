//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ENTITY_MEMBERS_HPP
#define NBDL_ENTITY_MEMBERS_HPP

#include <nbdl/fwd/entity_members.hpp>

#include <boost/hana/core/default.hpp>
#include <boost/hana/string.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <mpdef/map.hpp>
#include <mpdef/pair.hpp>
#include <type_traits>

namespace nbdl
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  namespace detail
  {
    template <typename Member>
    using entity_member_map_pair = mpdef::pair<member_name<Member>, Member>;

    template <typename Entity>
    constexpr auto entity_member_name_map = 
      mp_apply<mpdef::map,
      mp_transform<entity_member_map_pair,
        typename nbdl::entity_members_impl<Entity>::type
      >>{};
  }

  template <typename E>
  struct entity_members_impl
    : hana::default_
  { };

  template <typename Member>
  template <typename Entity>
  decltype(auto) get_member_fn<Member>::operator()(Entity&& e) const
  {
    if constexpr(hana::is_a<hana::string_tag, Member>())
    {
      using M = std::decay_t<decltype(
        hana::at_key(detail::entity_member_name_map<std::decay_t<Entity>>, Member{})
      )>;
      return std::forward<Entity>(e).*M::ptr;
    }
    else
    {
      return std::forward<Entity>(e).*Member::ptr;
    }
  };
}

#endif
