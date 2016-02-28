//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>
#include<def/builder/Path.hpp>
#include<def/builder/ProviderMap.hpp>
#include<def/builder/ProviderMeta.hpp>
#include<def/directives.hpp>
#include<Make.hpp>
#include<ProviderMap.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<const NAME##_t>;

namespace names {
  DEFINE_TYPE(Foo);
  DEFINE_TYPE(Provider1);
  DEFINE_TYPE(Provider2);
  DEFINE_TYPE(Provider1Name);
  DEFINE_TYPE(Provider2Name);
  DEFINE_TYPE(Entity_1_1);
  DEFINE_TYPE(Entity_1_2);
  DEFINE_TYPE(Entity_2_1);
}//names
namespace entity {
  struct E_1_1 {};
  struct E_1_2 {};
  struct E_2_1 {};
}//entity

constexpr auto entity_1_1 = builder::makeEntityMetaWithMap(
  builder::EntityMeta::keyMeta =
    builder::makeEntityKeyMetaWithMap(
      builder::EntityKeyMeta::entity  = hana::type_c<entity::E_1_1>,
      builder::EntityKeyMeta::key     = hana::type_c<int>
    ),
  builder::EntityMeta::membersMeta = hana::type_c<void>
);
constexpr auto entity_1_2 = builder::makeEntityMetaWithMap(
  builder::EntityMeta::keyMeta =
    builder::makeEntityKeyMetaWithMap(
      builder::EntityKeyMeta::entity  = hana::type_c<entity::E_1_2>,
      builder::EntityKeyMeta::key     = hana::type_c<int>
    ),
  builder::EntityMeta::membersMeta = hana::type_c<void>
);
constexpr auto entity_2_1 = builder::makeEntityMetaWithMap(
  builder::EntityMeta::keyMeta =
    builder::makeEntityKeyMetaWithMap(
      builder::EntityKeyMeta::entity  = hana::type_c<entity::E_2_1>,
      builder::EntityKeyMeta::key     = hana::type_c<int>
    ),
  builder::EntityMeta::membersMeta = hana::type_c<void>
);
constexpr auto entity_map = hana::make_map(
  hana::make_pair(names::Entity_1_1, entity_1_1),
  hana::make_pair(names::Entity_1_2, entity_1_2),
  hana::make_pair(names::Entity_2_1, entity_2_1)
);

constexpr auto access_points_1 = hana::make_tuple(
  builder::makeAccessPointMetaWithMap(
    builder::AccessPointMeta::name           = names::Foo,
    builder::AccessPointMeta::actions        = hana::make_tuple(nbdl_def::tag::Create),
    builder::AccessPointMeta::storeContainer = hana::type_c<void>,
    builder::AccessPointMeta::entityNames    = hana::make_tuple(names::Entity_1_1)
  ),
  builder::makeAccessPointMetaWithMap(
    builder::AccessPointMeta::name           = names::Foo,
    builder::AccessPointMeta::actions        = hana::make_tuple(nbdl_def::tag::Create),
    builder::AccessPointMeta::storeContainer = hana::type_c<void>,
    builder::AccessPointMeta::entityNames    = hana::make_tuple(names::Entity_1_2)
  )
);

constexpr auto access_points_2 = hana::make_tuple(
  builder::makeAccessPointMetaWithMap(
    builder::AccessPointMeta::name           = names::Foo,
    builder::AccessPointMeta::actions        = hana::make_tuple(nbdl_def::tag::Create),
    builder::AccessPointMeta::storeContainer = hana::type_c<void>,
    builder::AccessPointMeta::entityNames    = hana::make_tuple(names::Entity_2_1)
  )
);

template<typename>
struct show { };

int main()
{
  {
    constexpr auto providers_meta = hana::make_tuple(
      builder::makeProviderMetaWithMap(
        builder::ProviderMeta::provider     = names::Provider1,
        builder::ProviderMeta::name         = names::Provider1Name,
        builder::ProviderMeta::accessPoints = access_points_1
      ),
      builder::makeProviderMetaWithMap(
        builder::ProviderMeta::provider     = names::Provider2,
        builder::ProviderMeta::name         = names::Provider2Name,
        builder::ProviderMeta::accessPoints = access_points_2
      )
    );

    constexpr auto result = typename decltype(
        nbdl_def::builder::providerMap(entity_map, providers_meta))::type{};

    BOOST_HANA_CONSTANT_ASSERT(
      hana::decltype_(result[builder::path(entity_map, access_points_1[hana::int_c<0>])])
      == names::Provider1);
    BOOST_HANA_CONSTANT_ASSERT(
      hana::decltype_(result[builder::path(entity_map, access_points_1[hana::int_c<1>])])
      == names::Provider1);
    BOOST_HANA_CONSTANT_ASSERT(
        hana::decltype_(result[names::Provider1Name])
        == names::Provider1);
    BOOST_HANA_CONSTANT_ASSERT(
      hana::decltype_(result[builder::path(entity_map, access_points_2[hana::int_c<0>])])
      == names::Provider2);
    BOOST_HANA_CONSTANT_ASSERT(
      hana::decltype_(result[names::Provider2Name])
      == names::Provider2);
  }
}
