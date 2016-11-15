//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/entity_meta.hpp>
#include <nbdl/def/builder/entity_key_meta.hpp>
#include <nbdl/def/builder/path.hpp>
#include <nbdl/def/builder/provider_map.hpp>
#include <nbdl/def/builder/provider_meta.hpp>
#include <nbdl/def/directives.hpp>

#include <boost/hana.hpp>

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
  struct e_1_1 {};
  struct e_1_2 {};
  struct e_2_1 {};
}//entity

constexpr auto entity_1_1 = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e_1_1>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta = hana::type_c<void>
);
constexpr auto entity_1_2 = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e_1_2>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta = hana::type_c<void>
);
constexpr auto entity_2_1 = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e_2_1>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta = hana::type_c<void>
);
constexpr auto entity_map = hana::make_map(
  hana::make_pair(names::Entity_1_1, entity_1_1),
  hana::make_pair(names::Entity_1_2, entity_1_2),
  hana::make_pair(names::Entity_2_1, entity_2_1)
);

constexpr auto access_points_1 = hana::make_tuple(
  builder::make_access_point_meta_with_map(
    builder::access_point_meta::name            = names::Foo,
    builder::access_point_meta::actions         = hana::make_tuple(nbdl_def::tag::Create),
    builder::access_point_meta::store           = hana::type_c<void>,
    builder::access_point_meta::entity_names    = hana::make_tuple(names::Entity_1_1)
  ),
  builder::make_access_point_meta_with_map(
    builder::access_point_meta::name            = names::Foo,
    builder::access_point_meta::actions         = hana::make_tuple(nbdl_def::tag::Create),
    builder::access_point_meta::store           = hana::type_c<void>,
    builder::access_point_meta::entity_names    = hana::make_tuple(names::Entity_1_2)
  )
);

constexpr auto access_points_2 = hana::make_tuple(
  builder::make_access_point_meta_with_map(
    builder::access_point_meta::name            = names::Foo,
    builder::access_point_meta::actions         = hana::make_tuple(nbdl_def::tag::Create),
    builder::access_point_meta::store           = hana::type_c<void>,
    builder::access_point_meta::entity_names    = hana::make_tuple(names::Entity_2_1)
  )
);

template<typename>
struct show { };

int main()
{
  {
    constexpr auto providers_meta = hana::make_tuple(
      builder::make_provider_meta_with_map(
        builder::provider_meta::provider     = names::Provider1,
        builder::provider_meta::name         = names::Provider1Name,
        builder::provider_meta::access_points = access_points_1
      ),
      builder::make_provider_meta_with_map(
        builder::provider_meta::provider     = names::Provider2,
        builder::provider_meta::name         = names::Provider2Name,
        builder::provider_meta::access_points = access_points_2
      )
    );

    constexpr auto result = nbdl_def::builder::provider_map(entity_map, providers_meta);

    BOOST_HANA_CONSTANT_ASSERT(
      result[
        hana::make_tuple(
          builder::path(entity_map, access_points_1[hana::int_c<0>]),
          builder::path(entity_map, access_points_1[hana::int_c<1>]),
          names::Provider1Name
        )
      ]
      == names::Provider1);

    BOOST_HANA_CONSTANT_ASSERT(
      result[
        hana::make_tuple(
          builder::path(entity_map, access_points_2[hana::int_c<0>]),
          names::Provider2Name
        )
      ]
      == names::Provider2);
  }
}
