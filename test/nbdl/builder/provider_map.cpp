//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<store/HashMap.hpp>
#include<store_emitter/HashMap.hpp>
#include<def/builder/EnumerateAccessPoints.hpp>

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

    auto result = nbdl_def::builder::providerMap(def);

    constexpr auto provider_1_paths = hana::make_set(
      nbdl::path_type<int, entity::Root1, entity::Nested1>,
      nbdl::path_type<int, entity::Root1, entity::Nested2, entity::Nested3>
    );

    constexpr auto provider_2_paths = hana::make_set(
      nbdl::path_type<int, entity::Root2>,
      nbdl::path_type<int, entity::Root2, entity::Nested1>,
      nbdl::path_type<int, entity::Root2, entity::Nested2, entity::Nested3>
    );

    constexpr auto provider_3_paths = hana::make_set(
      nbdl::path_type<int, entity::Root3>;
      nbdl::path_type<int, entity::Root3, entity::Nested1>;
      nbdl::path_type<int, entity::Root3, entity::Nested2, entity::Nested3>;
    );

    constexpr auto match = hana::curry<2>(
      hana::compose(hana::equal, hana::partial(hana::at_key, result))
    );

    //todo the builder and the ProviderMap should have separate test files
    BOOST_HANA_CONSTANT_CHECK(hana::all_of(provider_1_paths, match(provider_1));
    BOOST_HANA_CONSTANT_CHECK(hana::all_of(provider_2_paths, match(provider_2));
    BOOST_HANA_CONSTANT_CHECK(hana::all_of(provider_3_paths, match(provider_3));
  }
}
