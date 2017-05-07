//
// Copyright Jason Rice 2017
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

TEST_CASE("Render store data then update", "[webui][renderer]")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  auto target = make_dom_test_equality(
    "<div class=\"foo\" >"
    "<div class=\"bar\" >"
    "I'm some static text content."
    " Here is some updated dynamic text."
    " More dynamic text."
    " More static text."
    "<span> Here is some optional text.</span>"
    "</div>"
    "</div>"
  );

  auto my_store = hana::make_map(
    hana::make_pair("key_1"_s, std::string("UPDATE ME"))
  , hana::make_pair("key_2"_s, std::string(" More dynamic text."))
  , hana::make_pair("key_3"_s, nbdl::optional<std::string>{})
  );

  constexpr auto spec =
    div(attr_class("foo"_s)
    , div(attr_class("bar"_s)
      , text_node("I'm some static text content."_s)
      , text_node(get("key_1"_s))
      , text_node(get("key_2"_s))
      , text_node(" More static text."_s)
      , match(
          get("key_3"_s)
        , when<std::string>(text_node(get("key_3"_s)))
        , when<>(div())
        )
      )
    );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  auto renderer = nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);

  my_store["key_1"_s] = std::string(" Here is some updated dynamic text.");
  my_store["key_3"_s] = std::string(" Here is some optional text.");

  nbdl::notify_state_change(renderer, hana::type_c<void>);

  CHECK(check_dom_equals());
}

TEST_CASE("Unsafely set innerHTML", "[webui][renderer]")
{
  using namespace nbdl::webui::html;

  auto target = make_dom_test_equality(
    "<pre>"
    "<span class=\"foo\">&amp;owner::member</span>"
    "</pre>"
  );

  auto my_store = hana::make_map();

  constexpr auto spec =
    pre(
      unsafe_set_inner_html("<span class=\"foo\">&amp;owner::member</span>"_s)
    );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);

  CHECK(check_dom_equals());
}
