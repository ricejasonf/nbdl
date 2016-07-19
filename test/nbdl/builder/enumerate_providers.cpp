//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <def/builder/EnumerateProviders.hpp>
#include <def/builder/EnumerateAccessPoints.hpp>
#include <def/builder/ProviderMeta.hpp>
#include <mpdef/list.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {

DEFINE_NAME(Provider1);
DEFINE_NAME(Provider2);

DEFINE_NAME(Provider1Name);
DEFINE_NAME(Provider2Name);

DEFINE_NAME(Root1);
DEFINE_NAME(Root2);

}//names

int main()
{
  using namespace nbdl_def;
  using builder::provider_meta;
  {

    constexpr auto access_points_def_1 = AccessPoints(
      AccessPoint(
        Name(names::Root1),
        EntityName(names::Root1),
        Actions(Create())
      )
    );

    constexpr auto access_points_def_2 = AccessPoints(
      AccessPoint(
        Name(names::Root2),
        EntityName(names::Root2),
        Actions(Create())
      )
    );

    constexpr auto def =
      Context(
        PrimaryKey(Type(hana::type_c<unsigned>)),
        Providers(
          Provider(
            Name(names::Provider1Name),
            Type(names::Provider1),
            access_points_def_1
          ),
          Provider(
            Name(names::Provider2Name),
            Type(names::Provider2),
            access_points_def_2
          )
        )
      );

    constexpr auto result = nbdl_def::builder::enumerate_providers(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      mpdef::make_list(
        builder::make_provider_meta_with_map(
          provider_meta::provider    = names::Provider1,
          provider_meta::name        = names::Provider1Name,
          provider_meta::access_points = builder::enumerate_access_points(access_points_def_1)
        ),
        builder::make_provider_meta_with_map(
          provider_meta::provider    = names::Provider2,
          provider_meta::name        = names::Provider2Name,
          provider_meta::access_points = builder::enumerate_access_points(access_points_def_2)
        )
      )
    );
  }
  // define a single provider
  {
    constexpr auto access_points_def_1 = AccessPoints(
      AccessPoint(
        Name(names::Root1),
        EntityName(names::Root1),
        Actions(Create())
      )
    );

    constexpr auto def =
      Context(
        PrimaryKey(Type(hana::type_c<unsigned>)),
        Provider(
          Name(names::Provider1Name),
          Type(names::Provider1),
          access_points_def_1
        )
      );

    constexpr auto result = nbdl_def::builder::enumerate_providers(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      mpdef::make_list(
        builder::make_provider_meta_with_map(
          provider_meta::provider       = names::Provider1,
          provider_meta::name           = names::Provider1Name,
          provider_meta::access_points = builder::enumerate_access_points(access_points_def_1)
        )
      )
    );
  }
}
