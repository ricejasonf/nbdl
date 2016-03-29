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
#include <nbdl/entity_members.hpp>
#include <nbdl/message.hpp>
#include<Path.hpp>
#include <mpdef/List.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<const NAME##_t>;

namespace names {
  DEFINE_TYPE(Foo);
  DEFINE_TYPE(Provider1);
  DEFINE_TYPE(Entity1);
}//names

struct my_system_message { };
struct provider1 { };
namespace entity {
  struct e1 { int x; };
}
namespace nbdl {
  NBDL_ENTITY(entity::e1, x);
} // nbdl

constexpr auto entity1_ = builder::make_entity_meta_with_map(
  builder::entity_meta::key_meta =
    builder::make_entity_key_meta_with_map(
      builder::entity_key_meta::entity  = hana::type_c<entity::e1>,
      builder::entity_key_meta::key     = hana::type_c<int>
    ),
  builder::entity_meta::members_meta = hana::type_c<void>
);

constexpr auto entity_map = mpdef::make_map(
  mpdef::make_tree_node(names::Entity1, entity1_)
);

int main()
{
  namespace channel = nbdl::message::channel;
  namespace action = nbdl::message::action;
  using builder::access_point_meta;
  using builder::provider_meta;
  {
    constexpr auto access_point = builder::make_access_point_meta_with_map(
      access_point_meta::name             = names::Foo,
      access_point_meta::actions          = hana::make_tuple(nbdl_def::tag::Create),
      access_point_meta::store_container  = hana::type_c<void>,
      access_point_meta::entity_names     = hana::make_tuple(names::Entity1)
    );
    using PathType = typename decltype(nbdl::path_type<int, entity::e1>)::type;
    constexpr auto provider_1 = builder::make_provider_meta_with_map(
      provider_meta::provider       = hana::type_c<provider1>,
      provider_meta::name           = names::Provider1,
      provider_meta::access_points  = mpdef::make_list(access_point)
    );

    constexpr auto make = builder::make_message_variants(entity_map, mpdef::make_list(provider_1));
    constexpr auto result = make(hana::type_c<my_system_message>);

    // upstream message variant
    BOOST_HANA_CONSTANT_ASSERT(
      hana::first(result)
        ==
      hana::type_c<
        nbdl::variant<
          my_system_message,
          hana::tuple<
            channel::upstream,
            action::create,
            PathType,
            const hana::optional<>,
            hana::optional<nbdl::uid>,
            hana::optional<entity::e1>,
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
        nbdl::variant<
          my_system_message,
          hana::tuple<
            channel::downstream,
            action::create,
            PathType,
            const hana::optional<>,
            hana::optional<nbdl::uid>,
            hana::optional<entity::e1>,
            const hana::optional<>
          >
        >
      >
    );
  }
}
