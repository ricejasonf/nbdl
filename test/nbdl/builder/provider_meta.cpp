//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/ProviderMeta.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

DEFINE_TYPE(Provider1);
DEFINE_TYPE(Provider1Name);
DEFINE_TYPE(AccessPoints);

int main()
{
  using namespace nbdl_def::builder;
  constexpr auto x = nbdl_def::builder::makeProviderMeta(Provider1, Provider1Name, AccessPoints);
  BOOST_HANA_CONSTANT_ASSERT(ProviderMeta::provider(x) == Provider1);
  BOOST_HANA_CONSTANT_ASSERT(ProviderMeta::name(x) == Provider1Name);
  BOOST_HANA_CONSTANT_ASSERT(ProviderMeta::accessPoints(x) == AccessPoints);
}
