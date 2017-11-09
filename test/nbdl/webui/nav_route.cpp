//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <assets/TestContext.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/string.hpp>
#include <catch.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/make_context.hpp>
#include <nbdl/string.hpp>
#include <nbdl/webui/nav_route.hpp>
#include <nbdl/webui/route_map.hpp>
#include <cstddef>

namespace hana = boost::hana;
using namespace hana::literals;
namespace message = nbdl::message;

namespace
{
  struct root { };

  struct my_route_1
  {
    std::size_t id_1;
    std::size_t id_2;
  };

  struct my_route_2
  {
    std::size_t id_1;
  };

  constexpr auto route_map = nbdl::webui::make_route_map(
    nbdl::webui::route_pair<root>(""_s)
  , nbdl::webui::route_pair<my_route_1>("my-route-1"_s)
  , nbdl::webui::route_pair<my_route_2>("my-route-2"_s)
  );

  struct context { }; // tag

  constexpr auto make_context_def = []
  {
    using namespace nbdl_def;

    return Context(
      NavRouteProducer(route_map)
    );
  };
}

namespace nbdl
{
  NBDL_EMPTY(root);
  NBDL_ENTITY(my_route_1, id_1, id_2);
  NBDL_ENTITY(my_route_2, id_1);

  template <>
  struct make_def_impl<::context>
  {
    static constexpr auto apply()
      -> decltype(make_context_def())
    { return {}; }
  };
}

TEST_CASE("Update and check the nav location using a nbdl::context", "[webui][nav_route]")
{
  auto ctx = nbdl::make_unique_context<context>();

  nbdl::apply_message(
    *ctx
  , message::make_upstream_update(
      hana::make_tuple("nav_route"_s)
    , message::no_uid
    , my_route_1{42, 84}
    )
  );

  {
    bool result = false;
    nbdl::match(
      *ctx
    , hana::make_tuple("nav_route"_s)
    , hana::overload_linearly(
        [&](my_route_1 const& r)
        {
          result = hana::equal(r, my_route_1{42, 84});
        }
      , nbdl::noop
      )
    );
    CHECK(result);
  }

  {
    bool result = false;
    nbdl::match(
      *ctx
    , hana::make_tuple("nav_route"_s, hana::type_c<nbdl::string>)
    , [&](nbdl::string const& str)
      {
        result = str == nbdl::string("/my-route-1/42/84");
      }
    );
    CHECK(result);
  }
}

TEST_CASE("Nav location is updated when browser changes location", "[webui][nav_route]")
{
  auto ctx = nbdl::make_unique_context<context>();

  EM_ASM({ window.history.pushState(null, null, "/"); });

  { // check initial location
    bool result = false;
    nbdl::match(
      *ctx
    , hana::make_tuple("nav_route"_s, hana::type_c<nbdl::string>)
    , [&](nbdl::string const& str)
      {
        result = str == nbdl::string("/");
      }
    );
    CHECK(result);
  }

  EM_ASM({ window.history.pushState(null, null, "/my-route-2/101"); });

  {
    bool result = false;
    nbdl::match(
      *ctx
    , hana::make_tuple("nav_route"_s)
    , hana::overload_linearly(
        [&](my_route_2 const& r)
        {
          result = hana::equal(r, my_route_2{101});
        }
      , nbdl::noop
      )
    );
    CHECK(result);
  }

  {
    bool result = false;
    nbdl::match(
      *ctx
    , hana::make_tuple("nav_route"_s, hana::type_c<nbdl::string>)
    , [&](nbdl::string const& str)
      {
        result = str == nbdl::string("/my-route-2/101");
      }
    );
    CHECK(result);
  }
}
