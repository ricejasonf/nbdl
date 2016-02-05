//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/EntityMeta.hpp>
#include<def/builder/EntityKeyMeta.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

DEFINE_TYPE(Entity1);
DEFINE_TYPE(Key1);

int main()
{
  using namespace nbdl_def;
  // EntityMeta
  {
    constexpr auto key_meta = builder::makeEntityKeyMeta(Entity1, Key1);
    constexpr auto member_meta = hana::int_c<42>;
    constexpr auto x = builder::makeEntityMeta(key_meta, member_meta);
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(x.keyMeta(), key_meta));
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(x.membersMeta(), member_meta));
  }

  // EntityKeyMeta
  {
    constexpr auto x = builder::makeEntityKeyMeta(Entity1, Key1);
    BOOST_HANA_CONSTANT_ASSERT(x.entity() == Entity1);
    BOOST_HANA_CONSTANT_ASSERT(x.key() == Key1);
  }

  // EntityMemberMeta
  {
    /*
     * TODO:
     *  EntityMemberMeta
     *  - same as all traits in Member.hpp
     *  - defaults to trait definitions
     */
  }
}
