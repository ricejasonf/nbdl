//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>
#include<def/builder/MessageVariants.hpp>
#include<def/builder/ProviderMeta.hpp>
#include<def/directives.hpp>
#include<macros/NBDL_ENTITY.hpp>
#include<Message.hpp>
#include<Path.hpp>
#include<mpdef/List.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<const NAME##_t>;

namespace names {
  DEFINE_TYPE(Foo);
  DEFINE_TYPE(Provider1);
  DEFINE_TYPE(Entity1);
}//names

struct MySystemMessage { };
struct Provider1 { };
namespace entity {
  struct E1 { int x; };
}
namespace nbdl {
  NBDL_ENTITY(entity::E1, x);
} // nbdl

constexpr auto entity1_ = builder::makeEntityMetaWithMap(
  builder::EntityMeta::keyMeta =
    builder::makeEntityKeyMetaWithMap(
      builder::EntityKeyMeta::entity  = hana::type_c<entity::E1>,
      builder::EntityKeyMeta::key     = hana::type_c<int>
    ),
  builder::EntityMeta::membersMeta = hana::type_c<void>
);

constexpr auto entity_map = mpdef::make_map(
  mpdef::make_tree_node(names::Entity1, entity1_)
);

int main()
{
  namespace channel = nbdl::message::channel;
  namespace action = nbdl::message::action;
  using builder::AccessPointMeta;
  using builder::ProviderMeta;
  {
    constexpr auto access_point = builder::makeAccessPointMetaWithMap(
      AccessPointMeta::name           = names::Foo,
      AccessPointMeta::actions        = hana::make_tuple(nbdl_def::tag::Create),
      AccessPointMeta::storeContainer = hana::type_c<void>,
      AccessPointMeta::storeEmitter   = hana::type_c<void>,
      AccessPointMeta::entityNames    = hana::make_tuple(names::Entity1)
    );
    using PathType = typename decltype(nbdl::path_type<int, entity::E1>)::type;
    constexpr auto provider_1 = builder::makeProviderMetaWithMap(
      ProviderMeta::provider      = hana::type_c<Provider1>,
      ProviderMeta::name          = names::Provider1,
      ProviderMeta::accessPoints  = mpdef::make_list(access_point)
    );

    constexpr auto make = builder::makeMessageVariants(entity_map, mpdef::make_list(provider_1));
    constexpr auto result = make(hana::type_c<MySystemMessage>);

    // upstream message variant
    BOOST_HANA_CONSTANT_ASSERT(
      hana::first(result)
        ==
      hana::type_c<
        nbdl::Variant<
          MySystemMessage,
          hana::tuple<
            channel::Upstream,
            action::Create,
            PathType,
            const hana::optional<>,
            hana::optional<nbdl::Uid>,
            hana::optional<entity::E1>,
            const hana::optional<>
          >
        >
      >
    );

    // downstream message variant
    BOOST_HANA_CONSTANT_ASSERT(
      hana::second(result)
        ==
      hana::type_c<
        nbdl::Variant<
          MySystemMessage,
          hana::tuple<
            channel::Downstream,
            action::Create,
            PathType,
            const hana::optional<>,
            hana::optional<nbdl::Uid>,
            hana::optional<entity::E1>,
            const hana::optional<>
          >
        >
      >
    );
  }
}
