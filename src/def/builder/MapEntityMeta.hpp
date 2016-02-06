//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_MAP_ENTITY_META_HPP
#define NBDL_DEF_MAP_ENTITY_META_HPP

#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>
#include<def/directives.hpp>

namespace nbdl_def {

namespace builder {

namespace hana = boost::hana;

struct MapEntityMeta
{
  //TODO make EnumerateMembers
  struct MembersMetaPlaceholder { };

  struct Helper
  {
    template<typename EntityDef>
    constexpr auto operator()(EntityDef) const
    {
      constexpr auto props = hana::second(EntityDef{});
      constexpr auto type = props[tag::Type];

      return hana::make_pair(
        props[tag::Name],
        builder::makeEntityMeta(
          builder::makeEntityKeyMeta(
            type,
            // TODO break out logic for PrimaryKey
            (
              hana::find(props, tag::PrimaryKey)
                | hana::reverse_partial(hana::find, tag::Type)
            ).value_or(hana::type_c<int>)
          ),
          hana::type_c<MembersMetaPlaceholder>
        )
      );
    }
  };

  // Def should be a pair<tag, tuple of Entity>
  template<typename Def>
  constexpr auto operator()(Def) const
  {
    constexpr auto entities = hana::second(Def{});
    return hana::unpack(entities, hana::make_map ^hana::on^ Helper{});
  }
};
constexpr MapEntityMeta mapEntityMeta{};

}//builder
}//nbdl_def
#endif
