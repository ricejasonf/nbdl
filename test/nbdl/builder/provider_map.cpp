//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMeta.hpp>
#include<def/builder/Path.hpp>
#include<def/builder/ProviderMap.hpp>
#include<def/builder/ProviderMeta.hpp>
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

constexpr auto entity_1_1 = builder::makeEntityMeta(hana::type_c<entity::E_1_1>, hana::type_c<int>);
constexpr auto entity_1_2 = builder::makeEntityMeta(hana::type_c<entity::E_1_2>, hana::type_c<int>);
constexpr auto entity_2_1 = builder::makeEntityMeta(hana::type_c<entity::E_2_1>, hana::type_c<int>);
constexpr auto entity_map = hana::make_map(
  hana::make_pair(names::Entity_1_1, entity_1_1),
  hana::make_pair(names::Entity_1_2, entity_1_2),
  hana::make_pair(names::Entity_2_1, entity_2_1)
);

constexpr auto access_points_1 = hana::make_tuple(
  builder::makeAccessPointMeta(
    names::Foo,
    hana::make_tuple(nbdl_def::tag::Create),
    hana::type_c<void>, //doesn't matter
    hana::type_c<void>, //doesn't matter
    hana::make_tuple(names::Entity_1_1)
  ),
  builder::makeAccessPointMeta(
    names::Foo,
    hana::make_tuple(nbdl_def::tag::Create),
    hana::type_c<void>, //doesn't matter
    hana::type_c<void>, //doesn't matter
    hana::make_tuple(names::Entity_1_2)
  )
);

constexpr auto access_points_2 = hana::make_tuple(
  builder::makeAccessPointMeta(
    names::Foo,
    hana::make_tuple(nbdl_def::tag::Create),
    hana::type_c<void>, //doesn't matter
    hana::type_c<void>, //doesn't matter
    hana::make_tuple(names::Entity_2_1)
  )
);

template<typename>
struct show { };

int main()
{
  {
    constexpr auto providers_meta = hana::make_tuple(
      builder::makeProviderMeta(
        names::Provider1,
        names::Provider1Name,
        access_points_1
      ),
      builder::makeProviderMeta(
        names::Provider2,
        names::Provider2Name,
        access_points_2
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
