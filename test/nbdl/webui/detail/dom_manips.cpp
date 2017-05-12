//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/pipe.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/run_sync.hpp>
#include <nbdl/ui_spec.hpp>
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

  auto result = nbdl::webui::detail::action_fn<begin, tag::element_t, div_tag>{}(body);

  CHECK(result["parentNode"].strictlyEquals(body));
}

TEST_CASE("end element", "[webui][dom_manips]")
{
  using nbdl::webui::detail::begin;
  using nbdl::webui::detail::end;
  auto body = make_body();
  auto result1 = emscripten::val::undefined();
  auto result2 = emscripten::val::undefined();

  nbdl::run_sync(
    nbdl::pipe(
      nbdl::webui::detail::action_fn<begin, tag::element_t, div_tag>{}
    , nbdl::tap([&](auto const& node) { result1 = node; })
    , nbdl::webui::detail::action_fn<end,   tag::element_t, div_tag>{}
    , nbdl::tap([&](auto const& node) { result2 = node; })
    )
  , body
  );

  CHECK( result2.strictlyEquals(body));
  CHECK(!result2.strictlyEquals(result1));
}

TEST_CASE("class attribute and text content", "[webui][dom_manips]")
{
  using nbdl::webui::detail::begin;
  using nbdl::webui::detail::end;
  using nbdl::webui::detail::action_fn;
  using nbdl::webui::detail::mut_action_fn;
  using nbdl::ui_spec::get;

  auto target = make_dom_test_equality(
    "<div class=\"foo\" >"
    "<div class=\"bar\" >"
    "I'm some static text content."
    "UPDATE ME"
    " More dynamic text."
    " More static text."
    "</div>"
    "</div>"
  );

  auto my_store = hana::make_map(
    hana::make_pair("key_1"_s, std::string("UPDATE ME"))
  , hana::make_pair("key_2"_s, std::string(" More dynamic text."))
  );

  using MyStore = decltype(my_store);


  auto mut_node_1 = mut_action_fn<tag::text_node_t, MyStore, decltype(get("key_1"_s))>(my_store);
  auto mut_node_2 = mut_action_fn<tag::text_node_t, MyStore, decltype(get("key_2"_s))>(my_store);

  nbdl::run_sync(
    nbdl::pipe(
      action_fn<begin, tag::element_t, div_tag>{}
    , action_fn<tag::attribute_t, decltype("class"_s), mpdef::list<decltype("foo"_s)>>{}
    , action_fn<begin, tag::element_t, div_tag>{}
    , action_fn<tag::attribute_t, decltype("class"_s), mpdef::list<decltype("bar"_s)>>{}
    , action_fn<tag::text_node_t, decltype("I'm some static text content."_s)>{}
    , std::ref(mut_node_1)
    , std::ref(mut_node_2)
    , action_fn<tag::text_node_t, decltype(" More static text."_s)>{}
    , action_fn<end, tag::element_t, div_tag>{}
    , action_fn<end, tag::element_t, div_tag>{}
    )
  , target
  );

  CHECK(check_dom_equals());
}
