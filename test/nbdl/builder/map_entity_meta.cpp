//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>
#include<def/builder/MapEntityMeta.hpp>
#include<def/directives.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {

DEFINE_NAME(Entity1);
DEFINE_NAME(Entity2);

}//names

struct E1 { };
struct E2 { };

int main()
{
  {
    using namespace nbdl_def;
    using builder::EntityMeta;
    using builder::EntityKeyMeta;

    constexpr auto entities =
      Entities(
        Entity(
          Name(names::Entity1),
          Type(hana::type_c<E1>)
        ),
        Entity(
          Name(names::Entity2),
          Type(hana::type_c<E2>)
        )
      );

    constexpr auto result = builder::mapEntityMeta(hana::second(entities));

    BOOST_HANA_CONSTANT_ASSERT(
      EntityKeyMeta::entity(EntityMeta::keyMeta(result[names::Entity1]))
        == hana::type_c<E1>);
    BOOST_HANA_CONSTANT_ASSERT(
      EntityKeyMeta::key(EntityMeta::keyMeta(result[names::Entity1]))
        == hana::type_c<int>);

    BOOST_HANA_CONSTANT_ASSERT(
      EntityKeyMeta::entity(EntityMeta::keyMeta(result[names::Entity2]))
        == hana::type_c<E2>);
    BOOST_HANA_CONSTANT_ASSERT(
      EntityKeyMeta::key(EntityMeta::keyMeta(result[names::Entity2]))
        == hana::type_c<int>);

  }
}
