//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/provider_map.hpp>
#include <nbdl/def/builder/provider_meta.hpp>
#include <nbdl/def/directives.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<const NAME##_t>; \
  struct NAME##_key {};

namespace names {
  DEFINE_TYPE(Foo);
  DEFINE_TYPE(Provider1);
  DEFINE_TYPE(Provider2);
  DEFINE_TYPE(Provider1Name);
  DEFINE_TYPE(Provider2Name);
  DEFINE_TYPE(Entity_1_1);
  DEFINE_TYPE(Entity_1_2);
  DEFINE_TYPE(Entity_2_1);
}//names

constexpr auto access_points_1 = mpdef::make_list(
  builder::make_access_point_meta_with_map(
    builder::access_point_meta::name            = names::Foo,
    builder::access_point_meta::actions         = mpdef::make_list(nbdl_def::tag::Create),
    builder::access_point_meta::store           = hana::type_c<void>,
    builder::access_point_meta::entities        = mpdef::make_list(names::Entity_1_1),
    builder::access_point_meta::path            = hana::type_c<hana::tuple<names::Entity_1_1_key>>
  ),
  builder::make_access_point_meta_with_map(
    builder::access_point_meta::name            = names::Foo,
    builder::access_point_meta::actions         = mpdef::make_list(nbdl_def::tag::Create),
    builder::access_point_meta::store           = hana::type_c<void>,
    builder::access_point_meta::entities        = mpdef::make_list(names::Entity_1_2),
    builder::access_point_meta::path            = hana::type_c<hana::tuple<names::Entity_1_2_key>>
  )
);

constexpr auto access_points_2 = mpdef::make_list(
  builder::make_access_point_meta_with_map(
    builder::access_point_meta::name            = names::Foo,
    builder::access_point_meta::actions         = mpdef::make_list(nbdl_def::tag::Create),
    builder::access_point_meta::store           = hana::type_c<void>,
    builder::access_point_meta::entities        = mpdef::make_list(names::Entity_2_1),
    builder::access_point_meta::path            = hana::type_c<hana::tuple< names::Entity_2_1_key>>
  )
);

template<typename>
struct show { };

int main()
{
  {
    constexpr auto providers_meta = hana::make_tuple(
      builder::make_provider_meta_with_map(
        builder::provider_meta::provider     = names::Provider1,
        builder::provider_meta::name         = names::Provider1Name,
        builder::provider_meta::access_points = access_points_1
      ),
      builder::make_provider_meta_with_map(
        builder::provider_meta::provider     = names::Provider2,
        builder::provider_meta::name         = names::Provider2Name,
        builder::provider_meta::access_points = access_points_2
      )
    );

    constexpr auto result = nbdl_def::builder::provider_map(providers_meta);

    BOOST_HANA_CONSTANT_ASSERT(
      hana::at_key(result, 
        mpdef::make_list(
          builder::access_point_meta::path(hana::at_c<0>(access_points_1)),
          builder::access_point_meta::path(hana::at_c<1>(access_points_1)),
          names::Provider1Name
        )
      )
      == names::Provider1);

    BOOST_HANA_CONSTANT_ASSERT(
      hana::at_key(result,
        mpdef::make_list(
          builder::access_point_meta::path(hana::at_c<0>(access_points_2)),
          names::Provider2Name
        )
      )
      == names::Provider2);
  }
}
