//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/ui_helper/path.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/map.hpp>
#include <catch.hpp>

namespace hana = boost::hana;

TEST_CASE("Use a ui_spec::path_t to visit a value in a store.", "[ui_helper][path]")
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
    nbdl::ui_helper::path(path_t<>{}
                        , store, [&](auto const& result)
    {
      CHECK(hana::equal(result, store));
      called = true;
    });
    CHECK(called);
  }

  {
    bool called = false;
    nbdl::ui_helper::path(path_t<get_t<hana::int_<0>>>{}
                        , store, [&](auto const& result)
    {
      CHECK(hana::equal(result, store[hana::int_c<0>]));
      called = true;
    });
    CHECK(called);
  }

  {
    bool called = false;
    nbdl::ui_helper::path(path_t<match_type<Type2, hana::int_<1>>
                               , match_type<Type1, hana::int_<0>>
                               >{}
                        , store, [&](auto const& result)
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
    nbdl::ui_helper::path(path_t<match_type<Type6, hana::int_<5>>
                               , match_type<Type5, hana::int_<4>>
                               , match_type<Type4, hana::int_<3>>
                               , match_type<Type3, hana::int_<2>>
                               , match_type<Type2, hana::int_<1>>
                               , match_type<Type1, hana::int_<0>>
                               >{}
                        , store, [&](auto const& result)
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
    nbdl::ui_helper::path(path_t<match_type<Type6, hana::int_<6>>
                               , match_type<Type5, hana::int_<4>>
                               , match_type<int, hana::int_<3>>
                               , match_type<Type3, hana::int_<2>>
                               , match_type<Type2, hana::int_<1>>
                               , match_type<Type1, hana::int_<0>>
                               >{}
                        , store, [&](auto const&)
    {
      called = true; // should not be called
    });
    CHECK(called == false);
  }
}
