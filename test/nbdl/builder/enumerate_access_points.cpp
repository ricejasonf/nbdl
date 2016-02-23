//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<store/HashMap.hpp>
#include<store_emitter/HashMap.hpp>
#include<def/builder/EnumerateAccessPoints.hpp>
#include<def/directives.hpp>
#include<mpdef/List.hpp>

#include<boost/hana.hpp>

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
        StoreContainer(hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        StoreEmitter(hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>),
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

    auto result = nbdl_def::builder::enumerateAccessPoints(def);

    constexpr auto expected = mpdef::make_list(
      // Root2
      builder::makeAccessPointMeta(
        names::Root2,
        mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
          mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        mpdef::make_list(names::Root2)
      ),
      // Root3
      builder::makeAccessPointMeta(
        names::Root3,
        mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        mpdef::make_list(names::Root3)
      ),
      // Root1/Nested1
      builder::makeAccessPointMeta(
        names::Nested1,
        mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
          mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        mpdef::make_list(names::Root1, names::Nested1)
      ),
      // Root1/Nested2/Nested3
      builder::makeAccessPointMeta(
        names::Nested3,
        mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        mpdef::make_list(
          names::Root1,
          names::Nested2,
          names::Nested3
        )
      ),
      // Root2/Nested1
      builder::makeAccessPointMeta(
        names::Nested1,
        mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
          mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        mpdef::make_list(
          names::Root2,
          names::Nested1
        )
      ),
      // Root2/Nested2/Nested3
      builder::makeAccessPointMeta(
        names::Nested3,
        mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        mpdef::make_list(
          names::Root2,
          names::Nested2,
          names::Nested3
        )
      ),
      // Root3/Nested1
      builder::makeAccessPointMeta(
        names::Nested1,
        mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
          mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
          mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        mpdef::make_list(
          names::Root3,
          names::Nested1
        )
      ),
      // Root3/Nested2/Nested3
      builder::makeAccessPointMeta(
        names::Nested3,
        mpdef::make_map(
          mpdef::make_tree_node(tag::Create,  mpdef::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        mpdef::make_list(
          names::Root3,
          names::Nested2,
          names::Nested3
        )
      )
    );

    BOOST_HANA_CONSTANT_ASSERT(result == expected);

    BOOST_HANA_CONSTANT_ASSERT(hana::at(result, hana::int_c<0>).name() == names::Root2);
    /* map is not default constructible :(
    BOOST_HANA_CONSTANT_ASSERT(hana::at(result, hana::int_c<0>).actions() ==
      mpdef::make_map(
        mpdef::make_tree_node(tag::Create,  mpdef::make_map()),
        mpdef::make_tree_node(tag::Read,    mpdef::make_map()),
        mpdef::make_tree_node(tag::Update,  mpdef::make_map()),
        mpdef::make_tree_node(tag::Delete,  mpdef::make_map())
      )
    );
    */
    BOOST_HANA_CONSTANT_ASSERT(hana::at(result, hana::int_c<0>).storeContainer() ==
      hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>
    );
    BOOST_HANA_CONSTANT_ASSERT(hana::at(result, hana::int_c<0>).storeEmitter() ==
      hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>
    );
    BOOST_HANA_CONSTANT_ASSERT(hana::at(result, hana::int_c<0>).entityNames() ==
      mpdef::make_list(names::Root2)
    );
  }
}
