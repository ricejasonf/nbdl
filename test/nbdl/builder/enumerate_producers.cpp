//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/def/builder/enumerate_producers.hpp>
#include <nbdl/def/builder/enumerate_access_points.hpp>
#include <nbdl/def/builder/producer_meta.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>; \
  struct NAME##_key {};

namespace names {

DEFINE_NAME(Producer1);
DEFINE_NAME(Producer2);

DEFINE_NAME(Producer1Name);
DEFINE_NAME(Producer2Name);

DEFINE_NAME(Root1);
DEFINE_NAME(Root2);

}//names

int main()
{
  using namespace nbdl_def;
  using builder::producer_meta;
  {

    constexpr auto access_points_def_1 = AccessPoints(
      AccessPoint(
        Name(names::Root1),
        Entity<names::Root1_t>,
        PathKey<names::Root1_key>,
        Actions(Create())
      )
    );

    constexpr auto access_points_def_2 = AccessPoints(
      AccessPoint(
        Name(names::Root2),
        Entity<names::Root2_t>,
        PathKey<names::Root2_key>,
        Actions(Create())
      )
    );

    constexpr auto def =
      Context(
        Producers(
          Producer(
            Name(names::Producer1Name),
            Type(names::Producer1),
            access_points_def_1
          ),
          Producer(
            Name(names::Producer2Name),
            Type(names::Producer2),
            access_points_def_2
          )
        )
      );

    constexpr auto result = nbdl_def::builder::enumerate_producers(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      mpdef::make_list(
        builder::make_producer_meta_with_map(
          producer_meta::producer    = names::Producer1,
          producer_meta::name        = names::Producer1Name,
          producer_meta::access_points = builder::enumerate_access_points(access_points_def_1)
        ),
        builder::make_producer_meta_with_map(
          producer_meta::producer    = names::Producer2,
          producer_meta::name        = names::Producer2Name,
          producer_meta::access_points = builder::enumerate_access_points(access_points_def_2)
        )
      )
    );
  }
  // define a single producer
  {
    constexpr auto access_points_def_1 = AccessPoints(
      AccessPoint(
        Name(names::Root1),
        Entity<names::Root1_t>,
        PathKey<names::Root1_key>,
        Actions(Create())
      )
    );

    constexpr auto def =
      Context(
        Producer(
          Name(names::Producer1Name),
          Type(names::Producer1),
          access_points_def_1
        )
      );

    constexpr auto result = nbdl_def::builder::enumerate_producers(def);

    BOOST_HANA_CONSTANT_ASSERT(result ==
      mpdef::make_list(
        builder::make_producer_meta_with_map(
          producer_meta::producer       = names::Producer1,
          producer_meta::name           = names::Producer1Name,
          producer_meta::access_points = builder::enumerate_access_points(access_points_def_1)
        )
      )
    );
  }
}
