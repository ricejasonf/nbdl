//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/entity_messages.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/entity_members.hpp>
#include <nbdl/message.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>; \
  struct NAME##_key {};

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
      access_point_meta::entities       = mpdef::make_list(names::Entity1),
      access_point_meta::path           = hana::type_c<hana::tuple<names::Entity1_key>>,
      access_point_meta::listen_paths   = mpdef::list<>{}
    );
    using Path = hana::tuple<names::Entity1_key>;
    using CreatePath = hana::tuple<hana::type<names::Entity1_key>>;

    BOOST_HANA_CONSTANT_ASSERT(
      builder::entity_messages(access_point)
        ==
      mpdef::make_list(
        hana::type_c<hana::tuple<
          channel::upstream,
          action::create,
          CreatePath,
          hana::optional<nbdl::uid>,
          hana::optional<names::Entity1_t>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::create,
          Path,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<names::Entity1_t>
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
          nbdl_def::Update(),
          nbdl_def::Delete()
        ),
      access_point_meta::store = hana::type_c<void>,
      access_point_meta::entities = mpdef::make_list(names::Entity1),
      access_point_meta::path = hana::type_c<hana::tuple<names::Entity1_key>>,
      access_point_meta::listen_paths   = mpdef::list<>{}
    );
    using Path = hana::tuple<names::Entity1_key>;
    using CreatePath = hana::tuple<hana::type<names::Entity1_key>>;

    BOOST_HANA_CONSTANT_ASSERT(
      builder::entity_messages(access_point)
        ==
      mpdef::make_list(
        hana::type_c<hana::tuple<
          channel::upstream,
          action::create,
          CreatePath,
          hana::optional<nbdl::uid>,
          hana::optional<names::Entity1_t>
        >>,
        hana::type_c<hana::tuple<
          channel::upstream,
          action::read,
          Path,
          hana::optional<nbdl::uid>
        >>,
        hana::type_c<hana::tuple<
          channel::upstream,
          action::update,
          Path,
          hana::optional<nbdl::uid>,
          hana::optional<names::Entity1_t>
        >>,
        hana::type_c<hana::tuple<
          channel::upstream,
          action::delete_,
          Path,
          hana::optional<nbdl::uid>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::create,
          Path,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<names::Entity1_t>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::read,
          Path,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<names::Entity1_t>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::update,
          Path,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<names::Entity1_t>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::delete_,
          Path,
          hana::optional<nbdl::uid>,
          hana::optional<bool>
        >>,
        hana::type_c<hana::tuple<
          channel::downstream,
          action::read,
          Path,
          hana::optional<nbdl::uid>,
          hana::optional<bool>,
          hana::optional<nbdl::not_found>
        >>
      )
    );
  }
}
