//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/message_api.hpp>
#include <nbdl/def/builder/producer_meta.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/entity_members.hpp>
#include <nbdl/message.hpp>
#include <nbdl/message_api.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>; \
  struct NAME##_key {};

namespace names
{
  DEFINE_TYPE(Foo);
  DEFINE_TYPE(Producer1);
  DEFINE_TYPE(Entity1);
}//names

namespace
{
  struct my_system_message { };
  struct producer1 { };
}

int main()
{
  namespace channel = nbdl::message::channel;
  namespace action = nbdl::message::action;
  using builder::access_point_meta;
  using builder::producer_meta;
  {
    constexpr auto access_point = builder::make_access_point_meta_with_map(
      access_point_meta::name             = names::Foo,
      access_point_meta::actions          = hana::make_map(nbdl_def::Create()),
      access_point_meta::store            = hana::type_c<void>,
      access_point_meta::entities         = mpdef::make_list(names::Entity1),
      access_point_meta::path             = hana::type_c<hana::tuple<names::Entity1_key>>
    );
    using Path = hana::tuple<names::Entity1_key>;
    using CreatePath = hana::tuple<hana::type<names::Entity1_key>>;
    constexpr auto producer_1 = builder::make_producer_meta_with_map(
      producer_meta::producer       = hana::type_c<producer1>,
      producer_meta::name           = names::Producer1,
      producer_meta::access_points  = mpdef::make_list(access_point)
    );

    struct context_mock
    {
      using message_api_meta = typename decltype(
        builder::make_message_api(mpdef::make_list(producer_1))
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
            hana::optional<names::Entity1_t>
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
            hana::optional<names::Entity1_t>
          >
        >
      >
    );
  }
}
