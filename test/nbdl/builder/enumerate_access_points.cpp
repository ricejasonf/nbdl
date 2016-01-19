//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<store/HashMap.hpp>
#include<store_emitter/HashMap.hpp>
#include<def/builder.hpp>
#include<def/builder/EnumerateAccessPoints.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {

DEFINE_NAME(Client1);
DEFINE_NAME(Client2);

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
        Clients(
          Client(
            Name(names::Client1),
            EntityName(names::Client1),
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
          Client(
            Name(names::Client2),
            EntityName(names::Client2),
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

    constexpr auto expected = hana::make_tuple(
      // Root2
      builder::makeAccessPointMeta(
        names::Root2,
        hana::make_map(
          hana::make_pair(tag::Create,  hana::make_map()),
          hana::make_pair(tag::Read,    hana::make_map()),
          hana::make_pair(tag::Update,  hana::make_map()),
          hana::make_pair(tag::Delete,  hana::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        hana::make_tuple(names::Root2)
      ),
      // Root3
      builder::makeAccessPointMeta(
        names::Root3,
        hana::make_map(
          hana::make_pair(tag::Create,  hana::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        hana::make_tuple(names::Root3)
      ),
      // Root1/Nested1
      builder::makeAccessPointMeta(
        names::Nested1,
        hana::make_map(
          hana::make_pair(tag::Create,  hana::make_map()),
          hana::make_pair(tag::Read,    hana::make_map()),
          hana::make_pair(tag::Update,  hana::make_map()),
          hana::make_pair(tag::Delete,  hana::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        hana::make_tuple(names::Root1, names::Nested1)
      ),
      // Root1/Nested2/Nested3
      builder::makeAccessPointMeta(
        names::Nested3,
        hana::make_map(
          hana::make_pair(tag::Create,  hana::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        hana::make_tuple(
          names::Root1,
          names::Nested2,
          names::Nested3
        )
      ),
      // Root2/Nested1
      builder::makeAccessPointMeta(
        names::Nested1,
        hana::make_map(
          hana::make_pair(tag::Create,  hana::make_map()),
          hana::make_pair(tag::Read,    hana::make_map()),
          hana::make_pair(tag::Update,  hana::make_map()),
          hana::make_pair(tag::Delete,  hana::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        hana::make_tuple(
          names::Root2,
          names::Nested1
        )
      ),
      // Root2/Nested2/Nested3
      builder::makeAccessPointMeta(
        names::Nested3,
        hana::make_map(
          hana::make_pair(tag::Create,  hana::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        hana::make_tuple(
          names::Root2,
          names::Nested2,
          names::Nested3
        )
      ),
      // Root3/Nested1
      builder::makeAccessPointMeta(
        names::Nested1,
        hana::make_map(
          hana::make_pair(tag::Create,  hana::make_map()),
          hana::make_pair(tag::Read,    hana::make_map()),
          hana::make_pair(tag::Update,  hana::make_map()),
          hana::make_pair(tag::Delete,  hana::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        hana::make_tuple(
          names::Root3,
          names::Nested1
        )
      ),
      // Root3/Nested2/Nested3
      builder::makeAccessPointMeta(
        names::Nested3,
        hana::make_map(
          hana::make_pair(tag::Create,  hana::make_map())
        ),
        hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>,
        hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>,
        hana::make_tuple(
          names::Root3,
          names::Nested2,
          names::Nested3
        )
      )
    );

    BOOST_HANA_CONSTANT_ASSERT(result == expected);

    BOOST_HANA_CONSTANT_ASSERT(result[hana::int_c<0>].name() == names::Root2);
    /* map is not default constructible :(
    BOOST_HANA_CONSTANT_ASSERT(result[hana::int_c<0>].actions() ==
      hana::make_map(
        hana::make_pair(tag::Create,  hana::make_map()),
        hana::make_pair(tag::Read,    hana::make_map()),
        hana::make_pair(tag::Update,  hana::make_map()),
        hana::make_pair(tag::Delete,  hana::make_map())
      )
    );
    */
    BOOST_HANA_CONSTANT_ASSERT(result[hana::int_c<0>].storeContainer() ==
      hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>
    );
    BOOST_HANA_CONSTANT_ASSERT(result[hana::int_c<0>].storeEmitter() ==
      hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>
    );
    BOOST_HANA_CONSTANT_ASSERT(result[hana::int_c<0>].entityNames() ==
      hana::make_tuple(names::Root2)
    );
  }
}
