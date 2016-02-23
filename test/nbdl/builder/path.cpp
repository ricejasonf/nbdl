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
  struct E1 {};
  struct E2 {};
  struct E3 {};
}

constexpr auto entity1_ = builder::makeEntityMeta(
  builder::makeEntityKeyMeta(hana::type_c<entity::E1>, hana::type_c<int>),
  hana::type_c<void>
);
constexpr auto entity2_ = builder::makeEntityMeta(
  builder::makeEntityKeyMeta(hana::type_c<entity::E2>, hana::type_c<int>),
  hana::type_c<void>
);
constexpr auto entity3_ = builder::makeEntityMeta(
  builder::makeEntityKeyMeta(hana::type_c<entity::E3>, hana::type_c<int>),
  hana::type_c<void>
);
constexpr auto entity_map = mpdef::make_map(
  mpdef::make_tree_node(names::Entity1, entity1_),
  mpdef::make_tree_node(names::Entity2, entity2_),
  mpdef::make_tree_node(names::Entity3, entity3_)
);

int main()
{
  {
    constexpr auto access_point = builder::makeAccessPointMeta(
      //name
      names::Foo,
      //actions
      hana::make_tuple(nbdl_def::tag::Create),
      //store
      hana::type_c<void>, //doesn't matter
      //storeEmitter
      hana::type_c<void>, //doesn't matter
      //entityNames
      hana::make_tuple(names::Entity1)
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::path(entity_map, access_point)
        ==
      nbdl::path_type<int, entity::E1>
    );
  }
  {
    constexpr auto access_point = builder::makeAccessPointMeta(
      //name
      names::Foo,
      //actions
      hana::make_tuple(nbdl_def::tag::Create),
      //store
      hana::type_c<void>, //doesn't matter
      //storeEmitter
      hana::type_c<void>, //doesn't matter
      //entityNames
      hana::make_tuple(names::Entity1, names::Entity2, names::Entity3)
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::path(entity_map, access_point)
        ==
      nbdl::path_type<int, entity::E1, entity::E2, entity::E3>
    );
  }
}
