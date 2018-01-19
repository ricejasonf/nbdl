//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/ui_helper/match_params_spec.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/map.hpp>
#include <catch.hpp>
#include <functional>
#include <string>

namespace hana      = boost::hana;
namespace ui_helper = nbdl::ui_helper;
namespace ui_spec   = nbdl::ui_spec;

namespace
{
  struct foo_t { };
  constexpr hana::type<foo_t> foo{};

  constexpr hana::string<'n', 'o', 't', 'h', 'i', 'n', 'g'> nothing_s;
}

TEST_CASE("match_params_spec resolves parameters which may be path specs.", "[ui_helper]")
{
  using nbdl::ui_spec::get;

  auto store = hana::make_map(
    hana::make_pair(foo, std::string("OK!"))
  );

  std::string result("Fail!");

  ui_helper::match_params_spec(
    std::ref(store)
  , mpdef::make_list(hana::int_c<0>, get(foo), hana::int_c<2>)
  , [&](hana::int_<0>, auto ref_a, hana::int_<2>)
    {
      result = ref_a;
    }
  );

  CHECK(result == std::string("OK!"));
}

TEST_CASE("Match values by type in match_params_spec.", "[ui_helper]")
{
  using nbdl::ui_spec::get;
  using nbdl::ui_spec::match;
  using nbdl::ui_spec::when;

  using string_option = nbdl::optional<std::string>;

  auto store = hana::make_map(
    hana::make_pair(foo, string_option{})
  );

  constexpr auto matcher = match(
    get(foo)
  , when<nbdl::nothing>(nothing_s)
  , when<std::string>(get)
  );

  std::string result("Fail!");

  auto run = [&]
  {
    ui_helper::match_params_spec(
      std::ref(store)
    , mpdef::make_list(hana::int_c<0>, matcher, hana::int_c<2>)
    , [&](hana::int_<0>, auto value, hana::int_<2>)
      {
        if constexpr(hana::is_a<hana::string_tag, decltype(value)>)
        {
          result = std::string(hana::to<char const*>(value));
        }
        else
        {
          result = value;
        }
      }
    );
  };
 
  run();
  CHECK(result == std::string("nothing"));

  store[foo] = std::string{"something"}; 

  run();
  CHECK(result == std::string("something"));
}

TEST_CASE("Match values by type in match_params_spec using match_when.", "[ui_helper]")
{
  using nbdl::ui_spec::get;
  using nbdl::ui_spec::match_when;
  using nbdl::ui_spec::when;

  constexpr auto make_nested_map = []
  {
    return hana::make_map(hana::make_pair(hana::int_c<1>, std::string("woof123")));
  };

  using nested_map = decltype(make_nested_map());
  using map_option = nbdl::optional<nested_map>;

  auto store = hana::make_map(hana::make_pair(hana::int_c<0>, map_option{make_nested_map()}));

  constexpr auto matcher = get(match_when<nested_map>(get(hana::int_c<0>)), hana::int_c<1>);

  std::string result("Fail!");

  auto run = [&]
  {
    ui_helper::match_params_spec(
      std::ref(store)
    , mpdef::make_list(matcher)
    , [&](auto value)
      {
        if constexpr(hana::is_a<hana::string_tag, decltype(value)>)
        {
          result = std::string(hana::to<char const*>(value));
        }
        else
        {
          result = value;
        }
      }
    );
  };
 
  run();
  CHECK(result == std::string("woof123"));
}

TEST_CASE("Apply params to a user function.", "[ui_helper]")
{
  using nbdl::ui_spec::get;

  auto store = hana::make_map(
    hana::make_pair(foo, std::string("OK!"))
  );

  std::string result("Fail!");

  ui_helper::match_params_spec(
    std::ref(store)
  , mpdef::make_list(ui_spec::apply(hana::make_tuple)(hana::int_c<0>, get(foo)), hana::int_c<2>)
  , [&](auto const& xs, hana::int_<2>)
    {
      CHECK(hana::equal(xs, hana::make_tuple(hana::int_c<0>, std::string("OK!"))));
      result = hana::at_c<1>(xs);
    }
  );

  CHECK(result == std::string("OK!"));
}
