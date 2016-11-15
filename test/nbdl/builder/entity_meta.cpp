//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/def/builder/entity_meta.hpp>
#include <nbdl/def/builder/entity_key_meta.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

DEFINE_TYPE(Entity1);
DEFINE_TYPE(Key1);

int main()
{
  using namespace nbdl_def;
  using builder::entity_meta;
  using builder::entity_key_meta;

  // entity_meta
  {
    constexpr auto key_meta = builder::make_entity_key_meta(Entity1, Key1);
    constexpr auto member_meta = hana::int_c<42>;
    constexpr auto x = builder::make_entity_meta(key_meta, member_meta);
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(entity_meta::key_meta(x), key_meta));
    BOOST_HANA_CONSTANT_ASSERT(hana::equal(entity_meta::members_meta(x), member_meta));
  }

  // entity_key_meta
  {
    constexpr auto x = builder::make_entity_key_meta(Entity1, Key1);
    BOOST_HANA_CONSTANT_ASSERT(entity_key_meta::entity(x) == Entity1);
    BOOST_HANA_CONSTANT_ASSERT(entity_key_meta::key(x) == Key1);
  }

  // entity_member_meta
  {
    /*
     * TODO:
     *  entity_member_meta
     *  - same as all traits in Member.hpp
     *  - defaults to trait definitions
     */
  }
}
