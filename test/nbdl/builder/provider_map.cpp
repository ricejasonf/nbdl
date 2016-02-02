//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/ProviderMap.hpp>
#include<Make.hpp>
#include<ProviderMap.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {

DEFINE_NAME(Provider1);
DEFINE_NAME(Provider2);

DEFINE_NAME(Provider1Name);
DEFINE_NAME(Provider2Name);

DEFINE_NAME(Path_1_1);
DEFINE_NAME(Path_1_2);
DEFINE_NAME(Path_1_3);

DEFINE_NAME(Path_2_1);
DEFINE_NAME(Path_2_2);

DEFINE_NAME(Path_3_1);

}//names

int main()
{
  {
    /*
     * TODO: This stuff has to be tested in the context builder.
     *
     *
     */
    using namespace nbdl_def;

    constexpr auto provider_1_paths = hana::make_tuple(
      Path_1_1,
      Path_1_2,
      Path_1_3
    );
    constexpr auto provider_2_paths = hana::make_tuple(
      Path_2_1,
      Path_2_2,
    );
    constexpr auto provider_3_paths = hana::make_tuple(
      Path_3_1
    );

    constexpr auto provider_metas = hana::make_tuple(
      builder::makeProviderMeta(
        Provider1,
        provider_1_paths
      ),
      builder::makeProviderMeta(
        Provider2,
        provider_2_paths
      ),
      builder::makeProviderMeta(
        Provider3,
        provider_3_paths
      )
    );

    constexpr auto result = nbdl_def::builder::providerMap(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      hana::make_type(nbdl::make<nbdl::ProviderMap>(
        hana::make_pair(hana::make_type(provider_1_paths), names::Provider1),
        hana::make_pair(hana::make_type(provider_2_paths), names::Provider2),
        hana::make_pair(hana::make_type(provider_3_paths), names::Provider3)
      ))
    );
  }
}
