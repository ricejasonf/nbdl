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
#include<EntityTraits.hpp>
#include<Listener.hpp>
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
  struct E1 { int x; };
  struct E2 { int x; };
  struct E3 { int x; };
}

namespace nbdl {
  NBDL_ENTITY(entity::E1, x);
  NBDL_ENTITY(entity::E2, x);
  NBDL_ENTITY(entity::E3, x);
}

constexpr auto entity1_ = builder::makeEntityMetaWithMap(
  builder::EntityMeta::keyMeta =
    builder::makeEntityKeyMetaWithMap(
      builder::EntityKeyMeta::entity  = hana::type_c<entity::E1>,
      builder::EntityKeyMeta::key     = hana::type_c<int>
    ),
  builder::EntityMeta::membersMeta =
    hana::type_c<void>
);
constexpr auto entity2_ = builder::makeEntityMetaWithMap(
  builder::EntityMeta::keyMeta =
    builder::makeEntityKeyMetaWithMap(
      builder::EntityKeyMeta::entity  = hana::type_c<entity::E2>,
      builder::EntityKeyMeta::key     = hana::type_c<int>
    ),
  builder::EntityMeta::membersMeta =
    hana::type_c<void>
);
constexpr auto entity3_ = builder::makeEntityMetaWithMap(
  builder::EntityMeta::keyMeta =
    builder::makeEntityKeyMetaWithMap(
      builder::EntityKeyMeta::entity  = hana::type_c<entity::E3>,
      builder::EntityKeyMeta::key     = hana::type_c<int>
    ),
  builder::EntityMeta::membersMeta =
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
    using builder::AccessPointMeta;
    using builder::EntityKeyMeta;
    using builder::EntityMeta;
    constexpr auto path_type_1 = nbdl::path_type<int, entity::E1>;
    constexpr auto path_type_2 = nbdl::path_type<int, entity::E1, entity::E2>;
    constexpr auto path_type_3 = nbdl::path_type<int, entity::E3>;

    constexpr auto listener_handler = hana::type_c<nbdl::ListenerHandlerDummy<>>;
    constexpr auto store_impl = hana::template_<nbdl::store::HashMap>;
    constexpr auto emitter_impl = hana::partial(
      hana::template_<nbdl::store_emitter::HashMap>,
      listener_handler
    );
    constexpr auto access_points = hana::make_tuple(
      builder::makeAccessPointMetaWithMap(
        AccessPointMeta::name           = names::Foo1,
        AccessPointMeta::actions        = hana::make_tuple(nbdl_def::tag::Create),
        AccessPointMeta::storeContainer = store_impl,
        AccessPointMeta::storeEmitter   = emitter_impl,
        AccessPointMeta::entityNames    = hana::make_tuple(names::Entity1)
      ),
      builder::makeAccessPointMetaWithMap(
        AccessPointMeta::name           = names::Foo2,
        AccessPointMeta::actions        = hana::make_tuple(nbdl_def::tag::Create),
        AccessPointMeta::storeContainer = store_impl,
        AccessPointMeta::storeEmitter   = emitter_impl,
        AccessPointMeta::entityNames    = hana::make_tuple(names::Entity1, names::Entity2)
      ),
      builder::makeAccessPointMetaWithMap(
        AccessPointMeta::name           = names::Foo3,
        AccessPointMeta::actions        = hana::make_tuple(nbdl_def::tag::Create),
        AccessPointMeta::storeContainer = store_impl,
        AccessPointMeta::storeEmitter   = emitter_impl,
        AccessPointMeta::entityNames    = hana::make_tuple(names::Entity3)
      )
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::storeMap(entity_map, access_points)
        ==
      hana::decltype_(
        hana::make_map(
          hana::make_pair(path_type_1,
            typename decltype(hana::template_<nbdl::Store>(
              store_impl(path_type_1),
              emitter_impl(path_type_1),
              listener_handler,
              path_type_1
            ))::type{}
          ),
          hana::make_pair(path_type_2,
            typename decltype(hana::template_<nbdl::Store>(
              store_impl(path_type_2),
              emitter_impl(path_type_2),
              listener_handler,
              path_type_2
            ))::type{}
          ),
          hana::make_pair(path_type_3,
            typename decltype(hana::template_<nbdl::Store>(
              store_impl(path_type_3),
              emitter_impl(path_type_3),
              listener_handler,
              path_type_3
            ))::type{}
          )
        )
      )
    );

  }
}
