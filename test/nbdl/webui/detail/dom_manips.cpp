//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/webui/detail/dom_manips.hpp>
#include <nbdl/webui/html.hpp>

#include <boost/hana.hpp>
#include <emscripten/val.h>
#include <catch.hpp>

namespace hana = boost::hana;
using namespace hana::literals;
using namespace std::string_literals;

namespace
{
  auto make_body()
  {
    emscripten::val::global("initDom")();
    auto body = emscripten::val::global("document")["body"];
    return body;
  }

  auto make_dom_test_equality(std::string expected)
  {
    emscripten::val::global("initDomEquality")(expected);
    return emscripten::val::global("test_target");
  }

  bool check_dom_equals()
  {
    return emscripten::val::global("checkDomEquals")().isTrue();
  }

  using div_tag = hana::string<'d', 'i', 'v'>;
}

namespace tag = nbdl::webui::html::tag;

TEST_CASE("begin element", "[webui][dom_manips]")
{
  using nbdl::webui::detail::begin;
  auto body = make_body();
  auto node = nbdl::webui::detail::action_fn<begin, tag::element_t, div_tag>{}(body);
  CHECK(node["parentNode"].strictlyEquals(body));
}

TEST_CASE("end element", "[webui][dom_manips]")
{
  using nbdl::webui::detail::begin;
  using nbdl::webui::detail::end;
  auto body = make_body();
  auto node1 = nbdl::webui::detail::action_fn<begin,  tag::element_t, div_tag>{}(body);
  auto node2 = nbdl::webui::detail::action_fn<end,    tag::element_t, div_tag>{}(node1);
  // end should return the parent
  CHECK( node2.strictlyEquals(body));
  CHECK(!node2.strictlyEquals(node1));
}

TEST_CASE("class attribute and text content", "[webui][dom_manips]")
{
  using nbdl::webui::detail::begin;
  using nbdl::webui::detail::end;
  using nbdl::webui::detail::action_fn;

  auto target = make_dom_test_equality(
    "<div class=\"foo\" >"
    "<div class=\"bar\" >"
    "I'm some text content."
    "</div>"
    "</div>"
  );

  // weird backwards nesting
  action_fn<end, tag::element_t, div_tag>{}(
  action_fn<end, tag::element_t, div_tag>{}(
  action_fn<tag::text_content_t, decltype("I'm some text content."_s)>{}(
  action_fn<tag::attribute_t, decltype("class"_s), decltype("bar"_s)>{}(
  action_fn<begin, tag::element_t, div_tag>{}(
  action_fn<tag::attribute_t, decltype("class"_s), decltype("foo"_s)>{}(
  action_fn<begin, tag::element_t, div_tag>{}(target)
  ))))));

  CHECK(check_dom_equals());
}
