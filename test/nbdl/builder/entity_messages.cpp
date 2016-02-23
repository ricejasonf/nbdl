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
  DEFINE_TYPE(Entity1);
}//names

namespace entity {
  struct E1 { int x; };
}
namespace nbdl {
  NBDL_ENTITY(entity::E1, x);
} // nbdl

constexpr auto entity1_ = builder::makeEntityMeta(
  builder::makeEntityKeyMeta(hana::type_c<entity::E1>, hana::type_c<int>),
  hana::type_c<void>
);

constexpr auto entity_map = mpdef::make_map(
  mpdef::make_tree_node(names::Entity1, entity1_)
);

int main()
{
  namespace channel = nbdl::message::channel;
  namespace action = nbdl::message::action;
  {
    constexpr auto access_point = builder::makeAccessPointMeta(
      //name
      names::Foo,
      //actions
      nbdl_def::Actions(nbdl_def::Create()),
      //store
      hana::type_c<void>, //doesn't matter
      //storeEmitter
      hana::type_c<void>, //doesn't matter
      //entityNames
      mpdef::make_list(names::Entity1)
    );
    using PathType = typename decltype(nbdl::path_type<int, entity::E1>)::type;

    BOOST_HANA_CONSTANT_ASSERT(
      builder::entityMessages(entity_map, access_point)
        ==
      mpdef::make_list(
        hana::type_c<hana::tuple<
          channel::Upstream,
          action::Create,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          hana::optional<entity::E1>,
          const hana::optional<>
        >>,
        hana::type_c<hana::tuple<
          channel::Downstream,
          action::Create,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          hana::optional<entity::E1>,
          const hana::optional<>
        >>
      )
    );
  }
  {
    constexpr auto access_point = builder::makeAccessPointMeta(
      //name
      names::Foo,
      //actions
      nbdl_def::Actions(
        nbdl_def::Create(),
        nbdl_def::Read(),
        nbdl_def::UpdateRaw(),
        nbdl_def::Delete()
      ),
      //store
      hana::type_c<void>, //doesn't matter
      //storeEmitter
      hana::type_c<void>, //doesn't matter
      //entityNames
      mpdef::make_list(names::Entity1)
    );
    using PathType = typename decltype(nbdl::path_type<int, entity::E1>)::type;

    BOOST_HANA_CONSTANT_ASSERT(
      builder::entityMessages(entity_map, access_point)
        ==
      mpdef::make_list(
        hana::type_c<hana::tuple<
          channel::Upstream,
          action::Create,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          hana::optional<entity::E1>,
          const hana::optional<>
        >>,
        hana::type_c<hana::tuple<
          channel::Downstream,
          action::Create,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          hana::optional<entity::E1>,
          const hana::optional<>
        >>,
        hana::type_c<hana::tuple<
          channel::Upstream,
          action::Read,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          const hana::optional<>, //has no payload
          const hana::optional<>
        >>,
        hana::type_c<hana::tuple<
          channel::Downstream,
          action::Read,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          hana::optional<entity::E1>,
          const hana::optional<>
        >>,
        hana::type_c<hana::tuple<
          channel::Upstream,
          action::UpdateRaw,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          hana::optional<entity::E1>,
          const hana::optional<>
        >>,
        hana::type_c<hana::tuple<
          channel::Downstream,
          action::UpdateRaw,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          hana::optional<entity::E1>,
          const hana::optional<>
        >>,
        hana::type_c<hana::tuple<
          channel::Upstream,
          action::Delete,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          const hana::optional<>,
          const hana::optional<>
        >>,
        hana::type_c<hana::tuple<
          channel::Downstream,
          action::Delete,
          PathType,
          const hana::optional<>,
          hana::optional<nbdl::Uid>,
          const hana::optional<>,
          const hana::optional<>
        >>
      )
    );
  }
}
