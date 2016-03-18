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
#include<mpdef/Map.hpp>
#include<mpdef/TreeNode.hpp>

namespace nbdl_def {

namespace builder {

namespace hana = boost::hana;

struct map_entity_meta_fn
{
  //TODO make EnumerateMembers
  struct members_meta_placeholder { };

  struct helper_fn
  {
    template<typename EntityDef>
    constexpr auto operator()(EntityDef) const
    {
      constexpr auto props = hana::second(EntityDef{});
      constexpr auto type = props[tag::Type];

      return mpdef::make_tree_node(
        hana::find(props, tag::Name).value_or(type),
        builder::make_entity_meta(
          builder::make_entity_key_meta(
            type,
            // TODO break out logic for PrimaryKey
            (
              hana::find(props, tag::PrimaryKey)
                | hana::reverse_partial(hana::find, tag::Type)
            ).value_or(hana::type_c<int>)
          ),
          hana::type_c<members_meta_placeholder>
        )
      );
    }
  };

  // Def should be a pair<tag, list of Entity>
  template<typename Def>
  constexpr auto operator()(Def) const
  {
    constexpr auto entities = Def{};
    return hana::unpack(entities, mpdef::make_map ^hana::on^ helper_fn{});
  }
};
constexpr map_entity_meta_fn map_entity_meta{};

}//builder
}//nbdl_def
#endif
