//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/store_map.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/entity_members.hpp>
#include <nbdl/null_store.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>; \
  struct NAME##_key {};

namespace names {
  DEFINE_TYPE(Foo1);
  DEFINE_TYPE(Foo2);
  DEFINE_TYPE(Foo3);
  DEFINE_TYPE(Entity1);
  DEFINE_TYPE(Entity2);
  DEFINE_TYPE(Entity3);
}//names

int main()
{
  {
    using builder::access_point_meta;

    constexpr auto path_type_1 = hana::type_c<hana::tuple<names::Entity1_key>>;
    constexpr auto path_type_2 = hana::type_c<hana::tuple<names::Entity1_key, names::Entity2_key>>;
    constexpr auto path_type_3 = hana::type_c<hana::tuple<names::Entity3_key>>;

    constexpr auto store_impl = hana::type_c<nbdl::null_store>;
    constexpr auto access_points = mpdef::make_list(
      builder::make_access_point_meta_with_map(
        access_point_meta::name           = names::Foo1,
        access_point_meta::actions        = mpdef::make_list(nbdl_def::tag::Create),
        access_point_meta::store          = store_impl,
        access_point_meta::entities       = mpdef::make_list(names::Entity1),
        access_point_meta::path           = path_type_1
      ),
      builder::make_access_point_meta_with_map(
        access_point_meta::name           = names::Foo2,
        access_point_meta::actions        = mpdef::make_list(nbdl_def::tag::Create),
        access_point_meta::store          = store_impl,
        access_point_meta::entities       = mpdef::make_list(names::Entity1, names::Entity2),
        access_point_meta::path           = path_type_2
      ),
      builder::make_access_point_meta_with_map(
        access_point_meta::name           = names::Foo3,
        access_point_meta::actions        = mpdef::make_list(nbdl_def::tag::Create),
        access_point_meta::store          = store_impl,
        access_point_meta::entities       = mpdef::make_list(names::Entity3),
        access_point_meta::path           = path_type_3
      )
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::store_map(access_points)
        ==
      hana::decltype_(
        hana::make_map(
          hana::make_pair(path_type_1, nbdl::null_store{}),
          hana::make_pair(path_type_2, nbdl::null_store{}),
          hana::make_pair(path_type_3, nbdl::null_store{})
        )
      )
    );

  }
}
