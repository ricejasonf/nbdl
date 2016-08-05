//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/webui/detail/dom_manips.hpp>
#include <nbdl/webui/html.hpp>

#include <emscripten/val.h>
#include <catch.hpp>

namespace hana = boost::hana;

namespace
{
  auto get_body()
  {
    auto body = emscripten::val::global("document")["body"];
    // clear all children
    while (true)
    {
      auto first_child = body["firstChild"];
      if (!first_child.template as<bool>())
        break;
      body.template call<void>("removeChild", body["firstChild"]);
    }
    return body;
  }

  using div_tag = hana::string<'d', 'i', 'v'>;
}

namespace tag = nbdl::webui::html::tag;

TEST_CASE("begin element", "[webui][dom_manips]")
{
  auto body = get_body();
  auto node = nbdl::webui::detail::begin_fn<tag::element_t, div_tag>{}(body);
  CHECK(node["parentNode"].strictlyEquals(body));
}

TEST_CASE("end element", "[webui][dom_manips]")
{
  auto body = get_body();
  auto node1 = nbdl::webui::detail::begin_fn<tag::element_t, div_tag>{}(body);
  auto node2 = nbdl::webui::detail::end_fn  <tag::element_t, div_tag>{}(node1);
  // end should return the parent
  CHECK( node2.strictlyEquals(body));
  CHECK(!node2.strictlyEquals(node1));
}
