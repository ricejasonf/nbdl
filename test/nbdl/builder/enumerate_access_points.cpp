//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <store/HashMap.hpp>
#include <def/builder/AccessPointMeta.hpp>
#include <def/builder/EnumerateAccessPoints.hpp>
#include <def/directives.hpp>
#include <mpdef/list.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {

DEFINE_NAME(Provider1);
DEFINE_NAME(Provider2);

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
          EntityName(names::Nested1),
          Actions(Create(), Read(), Update(), Delete())
        ),
        AccessPoint(
          Name(names::Nested2),
          EntityName(names::Nested2),
          AccessPoints(
            AccessPoint(
              Name(names::Nested3),
              EntityName(names::Nested3),
              Actions(Create())
            )
          )
        )
      );

    constexpr auto def =
      Context(
        PrimaryKey(Type(hana::type_c<unsigned>)),
        Store(hana::type_c<nbdl::null_store>),
        Providers(
          Provider(
            Name(names::Provider1),
            EntityName(names::Provider1),
            AccessPoints(
              AccessPoint(
                Name(names::Root1),
                EntityName(names::Root1),
                nested_accesspoints
              ),
              AccessPoint(
                Name(names::Root2),
                EntityName(names::Root2),
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
          Provider(
            Name(names::Provider2),
            EntityName(names::Provider2),
            AccessPoint(
              Name(names::Root3),
              EntityName(names::Root3),
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
        access_point_meta::entity_names = mpdef::make_list(names::Root2)
      ),
      // Root3
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Root3,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entity_names = mpdef::make_list(names::Root3)
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
        access_point_meta::entity_names = mpdef::make_list(names::Root1, names::Nested1)
      ),
      // Root1/Nested2/Nested3
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested3,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entity_names = mpdef::make_list(
          names::Root1,
          names::Nested2,
          names::Nested3
        )
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
        access_point_meta::entity_names = mpdef::make_list(
          names::Root2,
          names::Nested1
        )
      ),
      // Root2/Nested2/Nested3
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested3,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entity_names = mpdef::make_list(
          names::Root2,
          names::Nested2,
          names::Nested3
        )
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
        access_point_meta::entity_names = mpdef::make_list(
          names::Root3,
          names::Nested1
        )
      ),
      // Root3/Nested2/Nested3
      builder::make_access_point_meta_with_map(
        access_point_meta::name = names::Nested3,
        access_point_meta::actions = mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        access_point_meta::store =
          hana::type_c<nbdl::null_store>,
        access_point_meta::entity_names = mpdef::make_list(
          names::Root3,
          names::Nested2,
          names::Nested3
        )
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
      access_point_meta::entity_names(hana::at(result, hana::int_c<0>))
        ==
      mpdef::make_list(names::Root2)
    );
  }
}
