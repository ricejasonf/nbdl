//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/get_path.hpp>
#include <nbdl/ui_spec.hpp>

#include <awful.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <string>
#include <type_traits>

namespace hana = boost::hana;
namespace ui_spec = nbdl::ui_spec;
using namespace hana::literals;
using namespace std::string_literals;

namespace
{
  struct nc_string
  {
    std::string value;
    awful::noncopyable nocopy;

    nc_string(char const* c)
      : value(c)
      , nocopy()
    { }
    
    bool operator==(nc_string const& x) const
    {
      return value == x.value;
    }
  };
}

TEST_CASE("get_path gets nested value as a reference.", "[core]")
{
  auto my_store = hana::make_map(
    hana::make_pair("foo"_s, 
      hana::make_map(hana::make_pair("bar"_s, nc_string{"hello"}))
    )
  );

  auto& result = nbdl::get_path(my_store, hana::make_tuple("foo"_s, "bar"_s));
  CHECK(result == nc_string{"hello"});

  // Should not copy
  BOOST_HANA_CONSTANT_ASSERT(hana::equal(
    hana::type_c<
      ui_spec::detail::get_type_at_path<
        typename std::add_lvalue_reference<decltype(my_store)>::type
      , ui_spec::path_t<ui_spec::get_t<decltype("foo"_s), decltype("bar"_s)>>
      >
    >
  ,
    hana::type_c<nc_string&>
  ));

}
