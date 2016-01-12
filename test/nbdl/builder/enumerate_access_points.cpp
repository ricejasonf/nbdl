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
          Actions(Create(), Read(), Update(), Delete())
        ),
        AccessPoint(
          Name(names::Nested2),
          AccessPoints(
            AccessPoint(
              Name(names::Nested3),
              Actions(Create())
            )
          )
        )
      );

    constexpr auto def =
      Context(
        PrimaryKey(Type(hana::type_c<unsigned>)),
        Store(hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        StoreEmitter(hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>),
        Clients(
          Client(
            Name(names::Client1),
            AccessPoints(
              AccessPoint(
                Name(names::Root1),
                nested_accesspoints
              ),
              AccessPoint(
                Name(names::Root2),
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
            AccessPoint(
              Name(names::Root3),
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
      hana::make_map(
        hana::make_pair(tag::Name, names::Root2),
        hana::make_pair(tag::Actions,
          hana::make_map(
            hana::make_pair(tag::Create,  hana::make_map()),
            hana::make_pair(tag::Read,    hana::make_map()),
            hana::make_pair(tag::Update,  hana::make_map()),
            hana::make_pair(tag::Delete,  hana::make_map())
          )
        ),
        hana::make_pair(tag::Store,
          hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        hana::make_pair(tag::StoreEmitter,
          hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>)
      ),
      // Root3
      hana::make_map(
        hana::make_pair(tag::Name, names::Root3),
        hana::make_pair(tag::Actions,
          hana::make_map(
            hana::make_pair(tag::Create,  hana::make_map())
          )
        ),
        hana::make_pair(tag::Store,
          hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        hana::make_pair(tag::StoreEmitter,
          hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>)
      ),
      // Root1/Nested1
      hana::make_map(
        hana::make_pair(tag::Name, names::Nested1),
        hana::make_pair(tag::Actions,
          hana::make_map(
            hana::make_pair(tag::Create,  hana::make_map()),
            hana::make_pair(tag::Read,    hana::make_map()),
            hana::make_pair(tag::Update,  hana::make_map()),
            hana::make_pair(tag::Delete,  hana::make_map())
          )
        ),
        hana::make_pair(tag::Store,
          hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        hana::make_pair(tag::StoreEmitter,
          hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>)
      ),
      // Root1/Nested2/Nested3
      hana::make_map(
        hana::make_pair(tag::Name, names::Nested3),
        hana::make_pair(tag::Actions,
          hana::make_map(
            hana::make_pair(tag::Create,  hana::make_map())
          )
        ),
        hana::make_pair(tag::Store,
          hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        hana::make_pair(tag::StoreEmitter,
          hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>)
      ),
      // Root2/Nested1
      hana::make_map(
        hana::make_pair(tag::Name, names::Nested1),
        hana::make_pair(tag::Actions,
          hana::make_map(
            hana::make_pair(tag::Create,  hana::make_map()),
            hana::make_pair(tag::Read,    hana::make_map()),
            hana::make_pair(tag::Update,  hana::make_map()),
            hana::make_pair(tag::Delete,  hana::make_map())
          )
        ),
        hana::make_pair(tag::Store,
          hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        hana::make_pair(tag::StoreEmitter,
          hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>)
      ),
      // Root2/Nested2/Nested3
      hana::make_map(
        hana::make_pair(tag::Name, names::Nested3),
        hana::make_pair(tag::Actions,
          hana::make_map(
            hana::make_pair(tag::Create,  hana::make_map())
          )
        ),
        hana::make_pair(tag::Store,
          hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        hana::make_pair(tag::StoreEmitter,
          hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>)
      ),
      // Root3/Nested1
      hana::make_map(
        hana::make_pair(tag::Name, names::Nested1),
        hana::make_pair(tag::Actions,
          hana::make_map(
            hana::make_pair(tag::Create,  hana::make_map()),
            hana::make_pair(tag::Read,    hana::make_map()),
            hana::make_pair(tag::Update,  hana::make_map()),
            hana::make_pair(tag::Delete,  hana::make_map())
          )
        ),
        hana::make_pair(tag::Store,
          hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        hana::make_pair(tag::StoreEmitter,
          hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>)
      ),
      // Root3/Nested2/Nested3
      hana::make_map(
        hana::make_pair(tag::Name, names::Nested3),
        hana::make_pair(tag::Actions,
          hana::make_map(
            hana::make_pair(tag::Create,  hana::make_map())
          )
        ),
        hana::make_pair(tag::Store,
          hana::type_c<decltype(hana::template_<nbdl::store::HashMap>)>),
        hana::make_pair(tag::StoreEmitter,
          hana::type_c<decltype(hana::template_<nbdl::store_emitter::HashMap>)>)
      )
    );

    BOOST_HANA_CONSTANT_ASSERT(result == expected);
  }
}
