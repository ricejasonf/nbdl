//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/ui_helper/params_concat.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

TEST_CASE("Concatenate ui_spec params into an std::string.", "[ui_helper][params_concat]")
{
  constexpr hana::string<'f', 'o', 'o'> foo_s;
  constexpr hana::string<'b', 'a', 'r'> bar_s;
  constexpr hana::string<'b', 'a', 'z'> baz_s;

  auto store = hana::make_map(hana::make_pair(hana::int_c<0>, std::string(" Hello, world! ")));

  constexpr auto params = mpdef::make_list(
    foo_s
  , bar_s
  , baz_s
  , nbdl::ui_spec::get(hana::int_c<0>)
  , foo_s
  );

  std::string result = nbdl::ui_helper::params_concat(params, store).to_string();
  CHECK(result == std::string("foobarbaz Hello, world! foo"));
}
