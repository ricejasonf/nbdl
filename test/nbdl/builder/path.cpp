//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder.hpp>
#include<def/builder/Path.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {
  DEFINE_TYPE(Entity1);
  DEFINE_TYPE(Entity2);
  DEFINE_TYPE(Entity3);
}//names

DEFINE_TYPE(Entity1);
DEFINE_TYPE(Entity2);
DEFINE_TYPE(Entity3);

int main()
{
  {
    constexpr auto entity_map = hana::make_map(
      hana::make_pair(names::Entity1, Entity1),
      hana::make_pair(names::Entity2, Entity2),
      hana::make_pair(names::Entity3, Entity3)
    );

    constexpr auto access_point = hana::make_map(

  }
}
