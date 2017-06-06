//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/def/builder/access_point_meta.hpp>
#include <nbdl/def/builder/enumerate_access_points.hpp>
#include <nbdl/def/directives.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>; \
  struct NAME##_key {};

namespace names {

DEFINE_NAME(Producer1);
DEFINE_NAME(Producer2);

DEFINE_NAME(Root1);
DEFINE_NAME(Root2);
DEFINE_NAME(Root3);

DEFINE_NAME(Nested1);
DEFINE_NAME(Nested2);
DEFINE_NAME(Nested3);

}//names

int main()
{
  {
    using namespace nbdl_def;
    using builder::access_point_meta;
    constexpr auto nested_accesspoints =
      AccessPoints(
        AccessPoint(
          Name(names::Nested1),
          Entity<names::Nested1_t>,
          PathKey<names::Nested1_key>,
          Actions(Create(), Read(), Update(), Delete())
        ),
        AccessPoint(
          Name(names::Nested2),
          Entity<names::Nested2_t>,
          PathKey<names::Nested2_key>,
          AccessPoints(
            AccessPoint(
              Name(names::Nested3),
              Entity<names::Nested3_t>,
              PathKey<names::Nested3_key>,
              Actions(Create())
            )
          )
        )
      );

    constexpr auto def =
      Context(
        Store<nbdl::null_store>,
        Producers(
          Producer(
            Name(names::Producer1),
            AccessPoints(
              AccessPoint(
                Name(names::Root1),
                Entity<names::Root1_t>,
                PathKey<names::Root1_key>,
                nested_accesspoints
              ),
              AccessPoint(
                Name(names::Root2),
                Entity<names::Root2_t>,
                PathKey<names::Root2_key>,
                Actions(
                  Create(),
                  Read(),
                  Update(),
                  Delete()
                ),
                nested_accesspoints
              )
            )
          ),
          Producer(
            Name(names::Producer2),
            AccessPoint(
              Name(names::Root3),
              Entity<names::Root3_t>,
              PathKey<names::Root3_key>,
              Actions(
                Create()
              ),
              nested_accesspoints
            )
          )
        )
      );

    auto result = nbdl_def::builder::enumerate_access_points(def);

    constexpr auto expected = mpdef::make_list(
      // Root2
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Root2,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
          mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entities = mpdef::make_list(names::Root2),
        access_point_meta::path = hana::type_c<hana::tuple<names::Root2_key>>
      ),
      // Root3
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Root3,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        access_point_meta::store = hana::type_c<nbdl::null_store>,
        access_point_meta::entities = mpdef::make_list(names::Root3),
        access_point_meta::path = hana::type_c<hana::tuple<names::Root3_key>>
      ),
      // Root1/Nested1
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested1,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
          mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entities = mpdef::make_list(/*names::Root1,*/ names::Nested1),
        access_point_meta::path = hana::type_c<hana::tuple<names::Root1_key, names::Nested1_key>>
      ),
      // Root1/Nested2/Nested3
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested3,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entities = mpdef::make_list(
          //names::Root1,
          //names::Nested2,
          names::Nested3
        ),
        access_point_meta::path = hana::type_c<hana::tuple<
          names::Root1_key,
          names::Nested2_key,
          names::Nested3_key
        >>
      ),
      // Root2/Nested1
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested1,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
          mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entities = mpdef::make_list(
          //names::Root2,
          names::Nested1
        ),
        access_point_meta::path = hana::type_c<hana::tuple<
          names::Root2_key,
          names::Nested1_key
        >>
      ),
      // Root2/Nested2/Nested3
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested3,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entities = mpdef::make_list(
          //names::Root2,
          //names::Nested2,
          names::Nested3
        ),
        access_point_meta::path = hana::type_c<hana::tuple<
          names::Root2_key,
          names::Nested2_key,
          names::Nested3_key
        >>
      ),
      // Root3/Nested1
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested1,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
          mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entities = mpdef::make_list(
          //names::Root3,
          names::Nested1
        ),
        access_point_meta::path = hana::type_c<hana::tuple<
          names::Root3_key,
          names::Nested1_key
        >>
      ),
      // Root3/Nested2/Nested3
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested3,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entities = mpdef::make_list(
          //names::Root3,
          //names::Nested2,
          names::Nested3
        ),
        access_point_meta::path = hana::type_c<hana::tuple<
          names::Root3_key,
          names::Nested2_key,
          names::Nested3_key
        >>
      )
    );

    BOOST_HANA_CONSTANT_ASSERT(result == expected);

    BOOST_HANA_CONSTANT_ASSERT(access_point_meta::name(hana::at(result, hana::int_c<0>)) == names::Root2);
    BOOST_HANA_CONSTANT_ASSERT(access_point_meta::actions(hana::at(result, hana::int_c<0>)) ==
      mpdef::make_map(
        mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
        mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
        mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
        mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
      )
    );
    BOOST_HANA_CONSTANT_ASSERT(
      access_point_meta::store(hana::at(result, hana::int_c<0>))
        ==
      hana::type_c<nbdl::null_store>
    );
    BOOST_HANA_CONSTANT_ASSERT(
      access_point_meta::entities(hana::at(result, hana::int_c<0>))
        ==
      mpdef::make_list(names::Root2)
    );
    BOOST_HANA_CONSTANT_ASSERT(
      access_point_meta::path(hana::at(result, hana::int_c<0>))
        ==
      hana::type_c<hana::tuple<names::Root2_key>>
    );
  }
}
