//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/store_compose.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

TEST_CASE("Compose stores and perform operations on their nested values.", "[core]")
{
  static constexpr auto base_key = hana::typeid_([]{});
  constexpr auto key_1      = hana::typeid_([]{});
  constexpr auto key_2      = hana::typeid_([]{});
  constexpr auto key_3      = hana::typeid_([]{});
  constexpr auto key_4      = hana::typeid_([]{});
  constexpr auto key_5      = hana::typeid_([]{});

  auto base  = hana::make_map(hana::make_pair(base_key, std::string{"base_value"}));
  auto some_value_1 = std::string{"value_1"};
  auto some_value_2 = std::string{"value_2"};
  auto some_value_3 = std::string{"value_3"};
  auto some_value_4 = std::string{"value_4"};
  auto some_value_5 = std::string{"value_5"};

  auto store =  nbdl::store_compose(key_1, some_value_1,
                nbdl::store_compose(key_2, std::ref(some_value_2),
                nbdl::store_compose(key_3, std::ref(some_value_3),
                nbdl::store_compose(key_4, std::ref(some_value_4),
                nbdl::store_compose(key_5, std::ref(some_value_5),
                  base
                )))));

  constexpr auto append_z = [](auto& x) { x.push_back('z'); };

  hana::for_each(
    hana::make_tuple(key_1, key_2, key_3, key_4, key_5)
  , [&](auto key)
    {
      nbdl::apply_action(store, nbdl::store_composite_action(key, append_z));
    }
  );

  nbdl::apply_action(store, [](auto& x) { x[base_key].push_back('y'); });

  CHECK((base == hana::make_map(hana::make_pair(base_key, std::string{"base_value"}))));
  CHECK(some_value_1 == std::string("value_1"));
  CHECK(some_value_2 == std::string("value_2z"));
  CHECK(some_value_3 == std::string("value_3z"));
  CHECK(some_value_4 == std::string("value_4z"));
  CHECK(some_value_5 == std::string("value_5z"));

  std::string result_base{};
  std::string result_1{};
  std::string result_2{};
  std::string result_3{};
  std::string result_4{};
  std::string result_5{};

  nbdl::match(store, base_key, [&](auto& value) { result_base = value; });
  nbdl::match(store, key_1, [&](auto& value) { result_1 = value; });
  nbdl::match(store, key_2, [&](auto& value) { result_2 = value; });
  nbdl::match(store, key_3, [&](auto& value) { result_3 = value; });
  nbdl::match(store, key_4, [&](auto& value) { result_4 = value; });
  nbdl::match(store, key_5, [&](auto& value) { result_5 = value; });

  CHECK(result_base == "base_valuey");
  CHECK(result_1 == std::string("value_1z"));
  CHECK(result_2 == std::string("value_2z"));
  CHECK(result_3 == std::string("value_3z"));
  CHECK(result_4 == std::string("value_4z"));
  CHECK(result_5 == std::string("value_5z"));
}
