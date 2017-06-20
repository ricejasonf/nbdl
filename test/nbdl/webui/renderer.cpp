//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/variant.hpp>
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
    " More static text.1242"
    "<span> Here is some optional text.</span>"
    "</div>"
    "</div>"
  );

  auto my_store = hana::make_map(
    hana::make_pair("key_1"_s, std::string("UPDATE ME"))
  , hana::make_pair("key_2"_s, std::string(" More dynamic text."))
  , hana::make_pair("key_3"_s, nbdl::optional<std::string>{})
  , hana::make_pair("key_4"_s, 5)
  );

  constexpr auto spec =
    div(attr_class("foo"_s)
    , div(attr_class("bar"_s)
      , text_node("I'm some static text content."_s)
      , text_node(get("key_1"_s))
      , text_node(get("key_2"_s))
      , text_node(" More static text."_s)
      , text_node(hana::size_c<12>)
      , text_node(get("key_4"_s))
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
  my_store["key_4"_s] = 42;

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

TEST_CASE("Render dynamic attributes", "[webui][renderer]")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  auto target = make_dom_test_equality(
    "<div class=\"foo foo bar baz boo\" >"
    "</div>"
  );

  auto my_store = hana::make_map(
    hana::make_pair("key_1"_s, std::string("fooz"))
  , hana::make_pair("key_2"_s, nbdl::optional<std::string>{})
  );

  auto prepend = [](auto prefix) { return [=](auto path) { return concat(prefix, get(path)); };};

  auto spec =
    div(
      attr_class(concat(
        "foo "_s
      , get("key_1"_s)
      , " "_s
      , match(get("key_2"_s)
        , when<std::string>(prepend("bar "_s))
        , when<nbdl::nothing>(""_s)
        )
      , " boo"_s
      ))
    );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  auto renderer = nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);

  my_store["key_1"_s] = std::string("foo");
  my_store["key_2"_s] = std::string("baz");

  nbdl::notify_state_change(renderer, hana::type_c<void>);

  CHECK(check_dom_equals());
}

TEST_CASE("Match value using list of predicates", "[webui][renderer][ui_spec][match_if")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  auto target = make_dom_test_equality(
    "<div class=\"foo bar\" ></div>"
  );

  auto my_store = hana::make_map(
    hana::make_pair("key_1"_s, std::string("something else"))
  );

  auto spec =
    div(
      attr_class(concat(
        "foo "_s
      , match_if(
          get("key_1"_s)
        , cond(equal("blah"_s), "FAIL"_s)
        , cond(equal("blaz"_s), "FAIL"_s)
        , cond(equal("blar"_s), "FAIL"_s)
        , cond(equal("hello"_s), "bar"_s)
        , otherwise("FAIL"_s)
        )
      ))
    );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  auto renderer = nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);

  my_store["key_1"_s] = std::string("hello");

  nbdl::notify_state_change(renderer, hana::type_c<void>);

  CHECK(check_dom_equals());
}

#if 0 // Test is slow
TEST_CASE("Handle scaled up document structure", "[webui][renderer]")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  auto target = make_dom_test_equality("<div> </div>");

  auto my_store = hana::make_map();


  constexpr auto make_child = hana::always(div(attr_class("child"_s)));

  auto spec = hana::unpack(
    hana::make_range(hana::size_c<0>, hana::size_c<300>)
  , [&](auto ...i)
    {
      return div(attr_class("parent"_s), make_child(i)...);
    }
  );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);
}
#endif
