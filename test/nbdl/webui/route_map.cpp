//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/hana/equal.hpp>
#include <boost/hana/string.hpp>
#include <catch.hpp>
#include <nbdl/webui/route_map.hpp>
#include <nbdl/webui/set_route.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/string.hpp>
#include <cstddef>
#include <utility>

namespace hana = boost::hana;
using namespace hana::literals;
namespace message = nbdl::message;

namespace
{
  struct foo
  {
    std::size_t id;

    foo() = delete;
  };

  struct bar
  {
    nbdl::string name;

    bar() = delete;
  };

  struct baz
  {
    std::size_t id;
    nbdl::string name;

    baz() = delete;
  };

  struct boo_moo { };

  constexpr auto route_map = nbdl::webui::make_route_map(
    nbdl::webui::route_pair<>         ("")
  , nbdl::webui::route_pair<foo>      ("foo")
  , nbdl::webui::route_pair<bar>      ("bar")
  , nbdl::webui::route_pair<baz>      ("baz")
  , nbdl::webui::route_pair<boo_moo>  ("boo-moo")
  );

  using RouteMap = std::decay_t<decltype(route_map)>;
}

namespace nbdl
{
  NBDL_ENTITY(foo, id);
  NBDL_ENTITY(bar, name);
  NBDL_ENTITY(baz, id, name);
  NBDL_EMPTY(boo_moo);
}

TEST_CASE("Map route to variant", "[webui][route_map]")
{
  CHECK(hana::equal(route_map.to_variant(""_s)             , RouteMap::variant(""_s)));
  CHECK(hana::equal(route_map.to_variant(foo{42})          , RouteMap::variant(foo{42})));
  CHECK(hana::equal(route_map.to_variant(bar{"hello"})     , RouteMap::variant(bar{"hello"})));
  CHECK(hana::equal(route_map.to_variant(baz{42, "world"}) , RouteMap::variant(baz{42, "world"})));
}

TEST_CASE("Map route name to route type", "[webui][route_map]")
{
  CHECK((RouteMap::get_type("")        == hana::typeid_(""_s))); 
  CHECK((RouteMap::get_type("bar")     == hana::type_c<bar>)); 
  CHECK((RouteMap::get_type("baz")     == hana::type_c<baz>)); 
  CHECK((RouteMap::get_type("boo-moo") == hana::type_c<boo_moo>)); 
}

TEST_CASE("Map route to string", "[webui][route_map]")
{
  CHECK((route_map.to_string(""_s)               == nbdl::string("/")));
  CHECK((route_map.to_string(foo{42})            == nbdl::string("/foo/42")));
  CHECK((route_map.to_string(bar{"hello"})       == nbdl::string("/bar/hello")));
  CHECK((route_map.to_string(baz{42, "world"})   == nbdl::string("/baz/42/world")));
}

TEST_CASE("Map string to route", "[webui][route_map]")
{
  CHECK(hana::equal(route_map.from_string("/"), decltype(route_map)::variant{""_s}));

  CHECK(hana::equal(route_map.from_string("/foo/4"), decltype(route_map)::variant{foo{4}}));
  CHECK(hana::equal(route_map.from_string("/foo/42"), decltype(route_map)::variant{foo{42}}));
  CHECK(hana::equal(route_map.from_string("/foo/426"), decltype(route_map)::variant{foo{426}}));
  CHECK(hana::equal(route_map.from_string("/foo/moo"), decltype(route_map)::variant{}));
  CHECK(hana::equal(route_map.from_string("/foo/4/4"), decltype(route_map)::variant{}));

  CHECK(hana::equal(route_map.from_string("/bar/hello"), decltype(route_map)::variant{bar{"hello"}}));
  CHECK(hana::equal(route_map.from_string("/bar/42"), decltype(route_map)::variant{bar{"42"}}));
  CHECK(hana::equal(route_map.from_string("/bar/"), decltype(route_map)::variant{}));

  CHECK(hana::equal(
    route_map.from_string("/baz/4/world")
  , decltype(route_map)::variant{baz{4, "world"}}
  ));

  CHECK(hana::equal(
    route_map.from_string("/baz/42/w")
  , decltype(route_map)::variant{baz{42, "w"}}
  ));

  CHECK(hana::equal(
    route_map.from_string("/baz/42/")
  , decltype(route_map)::variant{}
  ));

  CHECK(hana::equal(
    route_map.from_string("/baz/0/nozeros")
  , decltype(route_map)::variant{}
  ));

  CHECK(hana::equal(
    route_map.from_string("/baz/12/toomany/args/world")
  , decltype(route_map)::variant{}
  ));

  CHECK(hana::equal(route_map.from_string("/boo-moo/"), decltype(route_map)::variant{boo_moo{}}));
  CHECK(hana::equal(route_map.from_string("/boo-moo/42"), decltype(route_map)::variant{}));

  CHECK(hana::equal(route_map.from_string("/platypus") , decltype(route_map)::variant{}));
  CHECK(hana::equal(route_map.from_string("") , decltype(route_map)::variant{}));
}

TEST_CASE("Make set_route for a route_map", "[webui][route_map]")
{
  constexpr auto set_route = nbdl::webui::make_set_route(route_map);

  {
    bool is_sent = false;
    auto send_message = [&](auto const& message)
    {
      is_sent = true;
      CHECK(hana::equal(message::get_payload(message), route_map.to_variant(""_s)));
    };
    set_route("")(send_message);
    CHECK(is_sent);
  }

  {
    bool is_sent = false;
    auto send_message = [&](auto const& message)
    {
      is_sent = true;
      CHECK(hana::equal(message::get_payload(message), route_map.to_variant(baz{42, "baz"})));
    };
    set_route("baz")(send_message, baz{42, "baz"});
    CHECK(is_sent);
  }

  {
    bool is_sent = false;
    auto send_message = [&](auto const& message)
    {
      is_sent = true;
      CHECK(hana::equal(message::get_payload(message), route_map.to_variant(boo_moo{})));
    };
    set_route("boo-moo")(send_message);
    CHECK(is_sent);
  }
}
