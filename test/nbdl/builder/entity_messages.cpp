//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMessages.hpp>
#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>
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
  DEFINE_TYPE(Entity1);
}//names

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
  using builder::access_point_meta;
  namespace channel = nbdl::message::channel;
  namespace action = nbdl::message::action;
  {
    constexpr auto access_point = builder::make_access_point_meta_with_map(
      access_point_meta::name           = names::Foo,
      access_point_meta::actions        = mpdef::make_map(nbdl_def::Create()),
      access_point_meta::store          = hana::type_c<void>,
      access_point_meta::entity_names   = mpdef::make_list(names::Entity1)
    );
    using PathType = typename decltype(nbdl::path_type<int, entity::e1>)::type;

    BOOST_HANA_CONSTANT_ASSERT(
      builder::entity_messages(entity_map, access_point)
        ==
      mpdef::make_list(
        hana::type_c<hana::tuple<
          channel::upstream,
          action::create,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<entity::e1>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::create,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<entity::e1>
        >>
      )
    );
  }
  {
    constexpr auto access_point = builder::make_access_point_meta_with_map(
      access_point_meta::name = names::Foo,
      access_point_meta::actions =
        mpdef::make_map(
          nbdl_def::Create(),
          nbdl_def::Read(),
          nbdl_def::UpdateRaw(),
          nbdl_def::Delete()
        ),
      access_point_meta::store = hana::type_c<void>,
      access_point_meta::entity_names    = mpdef::make_list(names::Entity1)
    );
    using PathType = typename decltype(nbdl::path_type<int, entity::e1>)::type;

    BOOST_HANA_CONSTANT_ASSERT(
      builder::entity_messages(entity_map, access_point)
        ==
      mpdef::make_list(
        hana::type_c<hana::tuple<
          channel::upstream,
          action::create,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<entity::e1>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::create,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<entity::e1>
        >>,
        hana::type_c<hana::tuple<
          channel::upstream,
          action::read,
          PathType,
          hana::optional<nbdl::uid>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::read,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<entity::e1>
        >>,
        hana::type_c<hana::tuple<
          channel::upstream,
          action::update_raw,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<entity::e1>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::update_raw,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<entity::e1>
        >>,
        hana::type_c<hana::tuple<
          channel::upstream,
          action::delete_,
          PathType,
          hana::optional<nbdl::uid>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::delete_,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<bool>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::read,
          PathType,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<nbdl::not_found>
        >>
      )
    );
  }
}
