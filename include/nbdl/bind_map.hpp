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
#include <functional>
#include <type_traits>
#include <utility>

namespace nbdl
{
  template <typename BindableMap, typename BindFn>
  constexpr auto bind_map_fn::operator()(BindableMap&& s, BindFn&& f) const
  {
    using Tag = hana::tag_of_t<BindableMap>;
    using Impl = bind_map_impl<Tag>;

    static_assert(nbdl::BindableMap<BindableMap>::value,
      "nbdl::bind_map(map, fn) requires 'map' to be a BindableMap");

    return Impl::apply(std::forward<BindableMap>(s), std::forward<BindFn>(f));
  };

  template <typename Tag, bool condition>
  struct bind_map_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  template <typename Tag>
  struct bind_map_impl<Tag, hana::when<nbdl::Entity<Tag>::value>>
  {
    template <typename Entity, typename BindFn>
    static constexpr auto apply(Entity&& e, BindFn&& f)
    {
      return hana::unpack(nbdl::entity_members<Entity>, [&](auto ...m)
      {
        return f(
          hana::make_pair(
            nbdl::member_name<decltype(m)>,
            std::ref(nbdl::get_member<decltype(m)>(e))
          )...
        );
      });
    }
  };

  template <>
  struct bind_map_impl<hana::map_tag>
  {
    template <typename BindableMap, typename BindFn>
    static constexpr auto apply(BindableMap&& map, BindFn&& f)
    {
      return hana::unpack(map, [&](auto&& ...pair)
      {
        static_assert(
          hana::all(mpdef::make_list(
            hana::is_convertible<hana::tag_of_t<decltype(hana::first(pair))>, char const*>{}...
          ))
        , "BindableMap keys must be convertible to char const*"
        );

        return (f(hana::make_pair(
          hana::first(pair),
          std::ref(hana::second(pair))
        )), ...);
      });
    }
  };
} // nbdl

#endif
