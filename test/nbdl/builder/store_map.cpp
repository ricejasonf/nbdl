//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>
#include<def/builder/StoreMap.hpp>
#include<def/directives.hpp>
#include <nbdl/entity_members.hpp>
#include<Path.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {
  DEFINE_TYPE(Foo1);
  DEFINE_TYPE(Foo2);
  DEFINE_TYPE(Foo3);
  DEFINE_TYPE(Entity1);
  DEFINE_TYPE(Entity2);
  DEFINE_TYPE(Entity3);
}//names

namespace entity {
  struct e1 { int x; };
  struct e2 { int x; };
  struct e3 { int x; };
}

namespace nbdl {
  NBDL_ENTITY(entity::e1, x);
  NBDL_ENTITY(entity::e2, x);
  NBDL_ENTITY(entity::e3, x);
}

constexpr auto entity1_ = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e1>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta =
    hana::type_c<void>
);
constexpr auto entity2_ = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e2>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta =
    hana::type_c<void>
);
constexpr auto entity3_ = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e3>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta =
    hana::type_c<void>
);
constexpr auto entity_map = hana::make_map(
  hana::make_pair(names::Entity1, entity1_),
  hana::make_pair(names::Entity2, entity2_),
  hana::make_pair(names::Entity3, entity3_)
);

int main()
{
  {
    using builder::access_point_meta;
    using builder::entity_key_meta;
    using builder::entity_meta;
    constexpr auto path_type_1 = nbdl::path_type<int, entity::e1>;
    constexpr auto path_type_2 = nbdl::path_type<int, entity::e1, entity::e2>;
    constexpr auto path_type_3 = nbdl::path_type<int, entity::e3>;

    constexpr auto store_impl = hana::template_<nbdl::store_container::hash_map>;
    constexpr auto access_points = hana::make_tuple(
      builder::make_access_point_meta_with_map(
        access_point_meta::name           = names::Foo1,
        access_point_meta::actions        = hana::make_tuple(nbdl_def::tag::Create),
        access_point_meta::store_container = store_impl,
        access_point_meta::entity_names    = hana::make_tuple(names::Entity1)
      ),
      builder::make_access_point_meta_with_map(
        access_point_meta::name           = names::Foo2,
        access_point_meta::actions        = hana::make_tuple(nbdl_def::tag::Create),
        access_point_meta::store_container = store_impl,
        access_point_meta::entity_names    = hana::make_tuple(names::Entity1, names::Entity2)
      ),
      builder::make_access_point_meta_with_map(
        access_point_meta::name           = names::Foo3,
        access_point_meta::actions        = hana::make_tuple(nbdl_def::tag::Create),
        access_point_meta::store_container = store_impl,
        access_point_meta::entity_names    = hana::make_tuple(names::Entity3)
      )
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::store_map(entity_map, access_points)
        ==
      hana::decltype_(
        hana::make_map(
          hana::make_pair(path_type_1,
            typename decltype(hana::template_<nbdl::store>(
              store_impl(path_type_1),
              path_type_1
            ))::type{}
          ),
          hana::make_pair(path_type_2,
            typename decltype(hana::template_<nbdl::store>(
              store_impl(path_type_2),
              path_type_2
            ))::type{}
          ),
          hana::make_pair(path_type_3,
            typename decltype(hana::template_<nbdl::store>(
              store_impl(path_type_3),
              path_type_3
            ))::type{}
          )
        )
      )
    );

  }
}
