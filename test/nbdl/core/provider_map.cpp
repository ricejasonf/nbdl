//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<Make.hpp>
#include<ProviderMap.hpp>

#include<catch.hpp>
#include<boost/hana.hpp>

namespace hana = boost::hana;

#define DEFINE_NAME(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

DEFINE_NAME(path_1_1)
DEFINE_NAME(path_1_2)
DEFINE_NAME(path_1_3)
DEFINE_NAME(path_1_4)
DEFINE_NAME(path_2_1)
DEFINE_NAME(path_2_2)
DEFINE_NAME(path_2_3)
DEFINE_NAME(path_2_4)
DEFINE_NAME(path_3_1)
DEFINE_NAME(path_3_2)
DEFINE_NAME(path_4_1)


template<int x>
struct ProviderDummy {
    int value;

    ProviderDummy()
        : value(0)
    { }

    ProviderDummy(ProviderDummy const&) = delete;
    ProviderDummy(ProviderDummy&& other)
        : value(other.value)
    { }
};

TEST_CASE("Map paths to providers.", "[provider_map]")
{
    constexpr auto provider_1_paths = hana::make_tuple(
      path_1_1,
      path_1_2,
      path_1_3,
      path_1_4
    );

    constexpr auto provider_2_paths = hana::make_tuple(
      path_2_1,
      path_2_2,
      path_2_3,
      path_2_4
    );

    constexpr auto provider_3_paths = hana::make_tuple(
      path_3_1,
      path_3_2
    );

    constexpr auto provider_4_paths = hana::make_tuple(
      path_4_1
    );

    auto result_map = nbdl::make<nbdl::ProviderMap>(
        hana::make_pair(hana::make_type(provider_1_paths), ProviderDummy<1>{}),
        hana::make_pair(hana::make_type(provider_2_paths), ProviderDummy<2>{}),
        hana::make_pair(hana::make_type(provider_3_paths), ProviderDummy<3>{}),
        hana::make_pair(hana::make_type(provider_4_paths), ProviderDummy<4>{})
    );

    // must be non-const
    result_map[path_1_1].value = 1;
    result_map[path_2_1].value = 2;
    result_map[path_3_1].value = 3;
    result_map[path_4_1].value = 4;

    auto match = hana::curry<2>(
      [&](auto const& expected, auto const& path) {
        return result_map[path].value == expected;
      }
    );

    //todo the builder and the ProviderMap should have separate test files
    CHECK(hana::all_of(provider_1_paths, match(1)));
    CHECK(hana::all_of(provider_2_paths, match(2)));
    CHECK(hana::all_of(provider_3_paths, match(3)));
    CHECK(hana::all_of(provider_4_paths, match(4)));
}
