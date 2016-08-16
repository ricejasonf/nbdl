//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <def/builder/AccessPointMeta.hpp>
#include <def/builder/EntityMeta.hpp>
#include <def/builder/EntityKeyMeta.hpp>
#include <def/builder/MessageApi.hpp>
#include <def/builder/ProviderMeta.hpp>
#include <def/directives.hpp>
#include <nbdl/entity_members.hpp>
#include <nbdl/message.hpp>
#include <nbdl/message_api.hpp>
#include <Path.hpp>
#include <mpdef/list.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<const NAME##_t>;

namespace names
{
  DEFINE_TYPE(Foo);
  DEFINE_TYPE(Provider1);
  DEFINE_TYPE(Entity1);
}//names

namespace
{
  struct my_system_message { };
  struct provider1 { };
}

namespace entity
{
  struct e1 { int x; };
}

namespace nbdl
{
  NBDL_ENTITY(entity::e1, x);
} // nbdl

namespace
{
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
}

int main()
{
  namespace channel = nbdl::message::channel;
  namespace action = nbdl::message::action;
  using builder::access_point_meta;
  using builder::provider_meta;
  {
    constexpr auto access_point = builder::make_access_point_meta_with_map(
      access_point_meta::name             = names::Foo,
      access_point_meta::actions          = hana::make_map(nbdl_def::Create()),
      access_point_meta::store            = hana::type_c<void>,
      access_point_meta::entity_names     = hana::make_tuple(names::Entity1)
    );
    using Path = typename decltype(nbdl::path_type<int, entity::e1>)::type;
    using CreatePath = typename decltype(Path::make_create_path_type())::type;
    constexpr auto provider_1 = builder::make_provider_meta_with_map(
      provider_meta::provider       = hana::type_c<provider1>,
      provider_meta::name           = names::Provider1,
      provider_meta::access_points  = mpdef::make_list(access_point)
    );

    struct context_mock
    {
      using message_api_meta = typename decltype(
        builder::make_message_api(entity_map, mpdef::make_list(provider_1))
      )::type;
    };

    constexpr nbdl::message_api<context_mock> api{};

    // use the variant functions to check that upstream
    // and downstream messages are partitioned properly
    constexpr auto upstream_type = hana::type_c<
      decltype(api.template make_upstream_variant<my_system_message>(nbdl::unresolved{}))
    >;
    constexpr auto downstream_type = hana::type_c<
      decltype(api.template make_downstream_variant<my_system_message>(nbdl::unresolved{}))
    >;

    // upstream message variant
    BOOST_HANA_CONSTANT_ASSERT(
      upstream_type
        ==
      hana::type_c<
        nbdl::variant<
          my_system_message,
          hana::tuple<
            channel::upstream,
            action::create,
            CreatePath,
            hana::optional<nbdl::uid>,
            hana::optional<entity::e1>
          >
        >
      >
    );

    // downstream message variant
    BOOST_HANA_CONSTANT_ASSERT(
      downstream_type
        ==
      hana::type_c<
        nbdl::variant<
          my_system_message,
          hana::tuple<
            channel::downstream,
            action::create,
            Path,
            hana::optional<nbdl::uid>,
            hana::optional<bool>,
            hana::optional<entity::e1>
          >
        >
      >
    );
  }
}
