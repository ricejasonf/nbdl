//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/enumerate_access_points.hpp>
#include <nbdl/def/builder/enumerate_producers.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/entity_members.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>; \
  struct NAME##_key {};

namespace names {
  DEFINE_TYPE(Producer1);
  DEFINE_TYPE(E1);
}//names

namespace entity {
  struct e1
  {
    int x;
  };
}

namespace nbdl {
  NBDL_ENTITY(entity::e1, x);
}

int main()
{
  {
    // The default store is `nbdl::null_store`
    using namespace nbdl_def;
    using nbdl_def::builder::access_point_meta;

    constexpr auto def =
      Context(
        Producer(
          Type<hana::type<void>>,
          Name(names::Producer1),
          AccessPoint(
            Name(names::E1),
            Entity<names::E1_t>,
            PathKey<names::E1_key>,
            Actions(Create())
          )
        )
      );
    constexpr auto result = hana::at_c<0>(builder::producer_meta::access_points(hana::at_c<0>(
      nbdl_def::builder::enumerate_producers(def)
    )));

    BOOST_HANA_CONSTANT_ASSERT(access_point_meta::store(result) == hana::type_c<nbdl::null_store>);
  }
  {
    // Specify the store in AccessPoint
    using namespace nbdl_def;
    using nbdl_def::builder::access_point_meta;

    struct test_store_tag { };
    constexpr auto def =
      Context(
        Producer(
          Type<hana::type<void>>,
          Name(names::Producer1),
          AccessPoint(
            Store<test_store_tag>,
            Name(names::E1),
            Entity<names::E1_t>,
            PathKey<names::E1_key>,
            Actions(Create())
          )
        )
      );
    constexpr auto result = hana::at_c<0>(builder::producer_meta::access_points(hana::at_c<0>(
      nbdl_def::builder::enumerate_producers(def)
    )));
    BOOST_HANA_CONSTANT_ASSERT(access_point_meta::store(result) == hana::type_c<test_store_tag>);
  }
}
