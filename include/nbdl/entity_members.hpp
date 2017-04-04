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
#include <boost/hana/core/to.hpp>
#include <boost/hana/string.hpp>
#include <mpdef/map.hpp>
#include <mpdef/pair.hpp>
#include <type_traits>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    template <typename Name, std::size_t ...N>
    constexpr hana::string<Name::value[N]...>
    member_name_to_string_helper(std::index_sequence<N...>)
    { return {}; }

    template <typename MemberName>
    struct member_name_to_string
    {
      //using Foo = typename hana::size_t<MemberName::length>::foo;
      using type = decltype(member_name_to_string_helper<MemberName>(
        std::make_index_sequence<MemberName::length>{}
      ));
    };

    struct make_entity_name_map_pair_fn
    {
      template <typename Member>
      constexpr auto operator()(Member m) const
      {
        return mpdef::make_pair(
          typename member_name_to_string<nbdl::member_name_impl<Member>>::type{}
        , m
        );
      }
    };

    // map hana::String : nbdl::member
    template <typename Entity>
    constexpr auto entity_member_name_map = hana::unpack(
      nbdl::entity_members<Entity>
    , hana::on(mpdef::make_map, make_entity_name_map_pair_fn{})
    );
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
} // nbdl

#endif
