//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/EntityMeta.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

DEFINE_TYPE(Entity1);
DEFINE_TYPE(Key1);

int main()
{
  constexpr auto x = nbdl_def::builder::makeEntityMeta(Entity1, Key1);
  BOOST_HANA_CONSTANT_ASSERT(x.entity() == Entity1);
  BOOST_HANA_CONSTANT_ASSERT(x.key() == Key1);
}
