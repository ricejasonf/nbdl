//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<mpdef/Map.hpp>
#include<mpdef/TreeNode.hpp>
#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>
#include<def/builder/Path.hpp>
#include<def/directives.hpp>
#include<Path.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {
  DEFINE_TYPE(Foo);
  DEFINE_TYPE(Entity1);
  DEFINE_TYPE(Entity2);
  DEFINE_TYPE(Entity3);
}//names

namespace entity {
  struct e1 {};
  struct e2 {};
  struct e3 {};
}

constexpr auto entity1_ = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e1>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta = hana::type_c<void>
);
constexpr auto entity2_ = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e2>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta = hana::type_c<void>
);
constexpr auto entity3_ = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e3>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta = hana::type_c<void>
);
constexpr auto entity_map = mpdef::make_map(
  mpdef::make_tree_node(names::Entity1, entity1_),
  mpdef::make_tree_node(names::Entity2, entity2_),
  mpdef::make_tree_node(names::Entity3, entity3_)
);

int main()
{
  using builder::access_point_meta;
  {
    constexpr auto access_point = builder::make_access_point_meta_with_map(
      access_point_meta::name           = names::Foo,
      access_point_meta::actions        = hana::make_tuple(nbdl_def::tag::Create),
      access_point_meta::store_container = hana::type_c<void>,
      access_point_meta::entity_names    = hana::make_tuple(names::Entity1)
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::path(entity_map, access_point)
        ==
      nbdl::path_type<int, entity::e1>
    );
  }
  {
    constexpr auto access_point = builder::make_access_point_meta_with_map(
      access_point_meta::name           = names::Foo,
      access_point_meta::actions        = hana::make_tuple(nbdl_def::tag::Create),
      access_point_meta::store_container = hana::type_c<void>,
      access_point_meta::entity_names    = hana::make_tuple(names::Entity1, names::Entity2, names::Entity3)
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::path(entity_map, access_point)
        ==
      nbdl::path_type<int, entity::e1, entity::e2, entity::e3>
    );
  }
}
