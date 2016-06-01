//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BIND_MAP_HPP
#define NBDL_BIND_MAP_HPP

#include <nbdl/fwd/bind_map.hpp>

#include <nbdl/concept/BindableMap.hpp>
#include <nbdl/concept/Entity.hpp>
#include <nbdl/entity_members.hpp>

#include <boost/hana/core/when.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/fwd/map.hpp>
#include <boost/hana/fwd/string.hpp>
#include <type_traits>
#include <utility>

namespace nbdl
{
  template <typename BindableMap, typename BindKeyValueFn>
  constexpr void bind_map_fn::operator()(BindableMap&& s, BindKeyValueFn&& f) const
  {
    using Tag = hana::tag_of_t<BindableMap>;
    using Impl = bind_map_impl<Tag>;

    static_assert(nbdl::BindableMap<BindableMap>::value,
      "nbdl::bind_map(map, fn) requires 'map' to be a BindableMap");

    return Impl::apply(std::forward<BindableMap>(s), std::forward<BindKeyValueFn>(f));
  };

  template <typename Tag, bool condition>
  struct bind_map_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr void apply(...) = delete;
  };

  template <typename Tag>
  struct bind_map_impl<Tag, hana::when<nbdl::Entity<Tag>::value>>
  {
    template <typename Entity, typename BindKeyValueFn>
    static void apply(Entity&& e, BindKeyValueFn&& f)
    {
      hana::for_each(nbdl::entity_members<Entity>, [&](auto m)
      {
        using Member = decltype(m);
        f(nbdl::member_name<Member>, nbdl::get_member<Member>(e));
      });
    }
  };

  template <>
  struct bind_map_impl<hana::map_tag>
  {
    template <typename T, typename = void>
    struct string_helper;

    template <typename T>
    struct string_helper<T, hana::when<std::is_same<decltype(T::value), char const*>::value>>
    {
      constexpr char const* operator()()
      {
        return T::value;
      }
    };

    template <typename T>
    struct string_helper<T, hana::when<hana::is_a<hana::string_tag, T>>>
    {
      constexpr char const* operator()()
      {
        return hana::to<char const*>(T{});
      }
    };

    template <typename BindableMap, typename BindKeyValueFn>
    static void apply(BindableMap&& map, BindKeyValueFn&& f)
    {
      hana::for_each(map, [&](auto&& pair)
      {
        f(string_helper<decltype(hana::first(pair))>{}(), hana::second(pair));
      });
    }
  };
} // nbdl

#endif
