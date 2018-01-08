//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/ui_helper/match_path_spec.hpp>
#include <nbdl/map_store.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <catch.hpp>
#include <functional>
#include <string>

namespace hana = boost::hana;

TEST_CASE("Use a ui_spec::path_t to visit a value in a store.", "[ui_helper]")
{
  // Nested maps
  auto store =  hana::make_map(hana::make_pair(hana::int_c< 0 >,
                hana::make_map(hana::make_pair(hana::int_c< 1 >,
                hana::make_map(hana::make_pair(hana::int_c< 2 >,
                hana::make_map(hana::make_pair(hana::int_c< 3 >,
                hana::make_map(hana::make_pair(hana::int_c< 4 >,
                hana::make_map(hana::make_pair(hana::int_c< 5 >,
                  42))))))))))));

  using Type0 = std::decay_t<decltype(store)>;
  using Type1 = std::decay_t<decltype(Type0{}[hana::int_c<0>])>;
  using Type2 = std::decay_t<decltype(Type1{}[hana::int_c<1>])>;
  using Type3 = std::decay_t<decltype(Type2{}[hana::int_c<2>])>;
  using Type4 = std::decay_t<decltype(Type3{}[hana::int_c<3>])>;
  using Type5 = std::decay_t<decltype(Type4{}[hana::int_c<4>])>;
  using Type6 = std::decay_t<decltype(Type5{}[hana::int_c<5>])>; // int

  using nbdl::ui_spec::path_t;
  using nbdl::ui_spec::get_t;
  using nbdl::ui_spec::match_type;

  {
    bool called = false;
    nbdl::ui_helper::match_path_spec(store, path_t<>{}, [&](auto const& result)
    {
      CHECK(hana::equal(result, store));
      called = true;
    });
    CHECK(called);
  }

  {
    bool called = false;
    nbdl::ui_helper::match_path_spec(store, path_t<get_t<hana::int_<0>>>{}, [&](auto const& result)
    {
      CHECK(hana::equal(result, store[hana::int_c<0>]));
      called = true;
    });
    CHECK(called);
  }

  {
    bool called = false;
    nbdl::ui_helper::match_path_spec( store
                                    , path_t<
                                        match_type<Type2, hana::int_<1>>
                                      , match_type<Type1, hana::int_<0>>
                                      >{}
                                    , [&](auto const& result)
    {
      CHECK(hana::equal(result, store[hana::int_c<0>]
                                     [hana::int_c<1>]
      ));
      called = true;
    });
    CHECK(called);
  }

  {
    bool called = false;
    nbdl::ui_helper::match_path_spec( store
                                    , path_t<
                                        match_type<Type6, hana::int_<5>>
                                      , match_type<Type5, hana::int_<4>>
                                      , match_type<Type4, hana::int_<3>>
                                      , match_type<Type3, hana::int_<2>>
                                      , match_type<Type2, hana::int_<1>>
                                      , match_type<Type1, hana::int_<0>>
                                    >{}
                                    , [&](auto const& result)
    {
      CHECK(hana::equal(result, store[hana::int_c<0>]
                                     [hana::int_c<1>]
                                     [hana::int_c<2>]
                                     [hana::int_c<3>]
                                     [hana::int_c<4>]
                                     [hana::int_c<5>]
      ));
      CHECK(result == 42);
      called = true;
    });
    CHECK(called);
  }

  {
    bool called = false;
    nbdl::ui_helper::match_path_spec( store
                                    , path_t<
                                        match_type<Type6, hana::int_<6>>
                                      , match_type<Type5, hana::int_<4>>
                                      , match_type<int, hana::int_<3>>
                                      , match_type<Type3, hana::int_<2>>
                                      , match_type<Type2, hana::int_<1>>
                                      , match_type<Type1, hana::int_<0>>
                                      >{}
                                      , [&](auto const&)
    {
      called = true; // should not be called
    });
    CHECK(called == false);
  }
}

TEST_CASE("Use key_at to use a value in the store as a key in a path.", "[ui_helper]")
{
  using map_store = nbdl::map_store<std::string, int>;
  using namespace nbdl::ui_spec;

  constexpr auto path = match_when<int>(get(hana::int_c<0>));

  auto store = hana::make_map(
    hana::make_pair(hana::int_c<0>, std::string("foo"))
  , hana::make_pair(hana::int_c<1>, map_store())
  );

  store[hana::int_c<1>].map[std::string("foo")] = 142;

  int result{0};

  nbdl::ui_helper::match_path_spec(
    store
  , get(hana::int_c<1>, key_at(path))
  , [&](int value) { result = value; }
  );
}
