//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <emscripten/val.h>
#define CONSOLE_LOG(x) emscripten::val::global("console").template call<void>("log", emscripten::val(x));

#include <nbdl/message.hpp>
#include <nbdl/variant.hpp>
#include <nbdl/webui/detail/dom_manips.hpp>
#include <nbdl/webui/html.hpp>
#include <nbdl_test/network_store.hpp>

#include <boost/hana.hpp>
#include <emscripten/val.h>
#include <catch.hpp>

namespace message = nbdl::message;
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

  auto prepend = [](auto prefix) { return [=](auto path) { return concat(prefix, path); };};

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

TEST_CASE("Match value using list of predicates", "[webui][renderer][ui_spec][match_if]")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  auto prepend = [](auto prefix) { return [=](auto path) { return concat(prefix, path); };};

  auto target = make_dom_test_equality(
    "<div class=\"foo prefix_hello\" ></div>"
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
        , cond(equal("hello"_s), prepend("prefix_"_s))
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

TEST_CASE("Handle click events.", "[webui][renderer][event_attribute]")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  auto target = make_dom_test_equality(R"HTML(<div id="click_me">clicked: 1</div>)HTML");

  auto my_store = nbdl_test::make_network_store(hana::make_map(
    hana::make_pair("click_count"_s, int{0})
  ));

  constexpr auto handle_click = [](auto send, int click_count)
  {
    send(message::make_upstream_update(
      hana::make_tuple("click_count"_s)
    , message::no_uid
    , click_count + 1
    ));
  };

  auto spec =
    div(
      attribute("id"_s, "click_me"_s)
    , on_click(handle_click, get("click_count"_s))
    , text_node("clicked: "_s)
    , text_node(get("click_count"_s))
    );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  auto renderer = nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);

  // trigger click
  emscripten::val::global("triggerEvent")(
    emscripten::val("click")
  , emscripten::val("click_me")
  );

  nbdl::notify_state_change(renderer, hana::type_c<void>);

  CHECK(check_dom_equals());
}

TEST_CASE("Add/Remove class based on predicate.", "[webui]")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  auto target = make_dom_test_equality(R"HTML(<div class="hello">whoa</div>)HTML");

  auto my_store = hana::make_map(
    hana::make_pair("has_stuff"_s, bool{true})
  );

  auto spec =
    div(
      add_class_if("hello"_s, get("has_stuff"_s), hana::id)
    , add_class_if("FAIL"_s,  get("has_stuff"_s), hana::not_)
    , text_node("whoa"_s)
    );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  auto renderer = nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);

  CHECK(check_dom_equals());
}

TEST_CASE("Add/Remove class based on matching a type.", "[webui]")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  auto target = make_dom_test_equality(R"HTML(<div class="hello">whoa</div>)HTML");

  auto my_store = nbdl::variant<float, int>{int{42}};

  auto spec =
    div(
      add_class_when<int>("hello"_s, get())
    , add_class_when<float>("FAIL"_s,  get())
    , text_node("whoa"_s)
    );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  auto renderer = nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);

  CHECK(check_dom_equals());
}

TEST_CASE("Render a Container in a Store with for_each", "[webui]")
{
  using namespace nbdl::webui::html;
  using namespace nbdl::ui_spec;

  //auto target = make_dom_test_equality(R"HTML(<div><div class="row">Row #1</div><div class="row">Row #2</div><div class="row">Row #3</div><div class="row">Row #4</div><div class="row">Row #5</div><div class="row">Row #6</div><div class="row">Row #7</div><div class="row">Row #8</div></div>)HTML");
  auto target = make_dom_test_equality(R"HTML(<div><div><span><div class="row">Row #1</div></span><span><div class="row">Row #2</div></span><span><div class="row">Row #3</div></span><span><div class="row">Row #4</div></span><span><div class="row">Row #5</div></span><span><div class="row">Row #6</div></span><span><div class="row">Row #7</div></span><span><div class="row">Row #8</div></span></div><div>CLICK THIS BEFORE IT CHANGES!!</div></div>)HTML");

  constexpr auto make_row = [](std::string text)
  {
    return hana::make_map(hana::make_pair("text"_s, text));
  };

  using Vector = std::vector<decltype(make_row(std::string{}))>;

  auto my_store = nbdl_test::make_network_store(hana::make_map(
    hana::make_pair("last_clicked"_s, std::string{"FAIL"})
  , hana::make_pair("column"_s, Vector{
      make_row("Row #1")
    , make_row("Row #2")
    , make_row("Row #3")
    , make_row("Row #4")
    , make_row("Row #5")
    , make_row("Row #6")
    , make_row("Row #7")
    , make_row("CLICK THIS BEFORE IT CHANGES!!")
    })
  ));

  static constexpr auto handle_click = [](auto send, std::string text)
  {
    send(message::make_upstream_update(
      hana::make_tuple("last_clicked"_s)
    , message::no_uid
    , text
    ));
  };

  static constexpr auto click_row = [](auto x)
  {
    return div(
      attr_class("row"_s)
    , attribute("id"_s, "click_me"_s)
    , on_click(handle_click, get(x, "text"_s))
    , text_node(get(x, "text"_s))
    );
  };

  static constexpr auto row = [](auto x) {
    return div(
      attr_class("row"_s)
    , text_node(get(x, "text"_s))
    );
  };

  constexpr auto spec = div(
    div(
      for_each(get("column"_s), [](auto x)
      {
        return match_if(
          get(x, "text"_s)
        , cond(equal("CLICK THIS BEFORE IT CHANGES!!"_s), click_row)
        , otherwise(row)
        );
      })
    )
  , div(text_node(get("last_clicked"_s)))
  );

  using renderer_tag = nbdl::webui::renderer<decltype(spec)>;
  auto renderer = nbdl::make_state_consumer<renderer_tag>(std::ref(my_store), target);

  // trigger click
  emscripten::val::global("triggerEvent")(
    emscripten::val("click")
  , emscripten::val("click_me")
  );
  nbdl::notify_state_change(renderer, hana::type_c<void>);

  my_store.store["column"_s][7]["text"_s] = "Row #8";
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
