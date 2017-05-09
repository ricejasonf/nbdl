//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/webui/detail/flatten_spec.hpp>
#include <nbdl/webui/html.hpp>

#include <boost/hana.hpp>

namespace hana  = boost::hana;
using namespace hana::literals;

#define CHECK_TYPE_EQUAL(RESULT, EXPECTED) BOOST_HANA_CONSTANT_CHECK(hana::equal( \
  hana::typeid_(RESULT), hana::typeid_(EXPECTED) \
));

int main()
{
  using namespace nbdl::ui_spec;
  using namespace nbdl::webui::html;
  namespace tag = nbdl::webui::html::tag;
  using nbdl::webui::detail::action_fn;

  auto begin = [](auto ...xs)
  {
    return action_fn<nbdl::webui::detail::begin, decltype(xs)...>{};
  };

  auto end = [](auto ...xs)
  {
    return action_fn<nbdl::webui::detail::end, decltype(xs)...>{};
  };

  constexpr hana::string<'d', 'i', 'v'> div_s{};

  {
    constexpr auto spec = div();

    constexpr auto result = nbdl::webui::detail::flatten_spec(spec);
    constexpr auto expected = mpdef::make_list(
      begin(tag::element_t{}, div_s),
      end(tag::element_t{}, div_s)
    );
    (void)result;
    (void)expected;

    CHECK_TYPE_EQUAL(result, expected);
  }
  {
    constexpr auto spec = div(div(), div());

    constexpr auto result = nbdl::webui::detail::flatten_spec(spec);
    constexpr auto expected = mpdef::make_list(
      begin(tag::element_t{}, div_s),
        begin(tag::element_t{}, div_s),
        end  (tag::element_t{}, div_s),
        begin(tag::element_t{}, div_s),
        end  (tag::element_t{}, div_s),
      end  (tag::element_t{}, div_s)
    );

    (void)result;
    (void)expected;

    CHECK_TYPE_EQUAL(result, expected);
  }
  {
    constexpr auto spec = div(
      attr_class("foo"_s),
      div(
        text_node("Hello World!"_s)
      )
    );

    constexpr auto result = nbdl::webui::detail::flatten_spec(spec);
    constexpr auto expected = mpdef::make_list(
      begin(tag::element_t{}, div_s),
        action_fn<tag::attribute_t, decltype("class"_s), mpdef::list<decltype("foo"_s)>>{},
        begin(tag::element_t{}, div_s),
          action_fn<tag::text_node_t, decltype("Hello World!"_s)>{},
        end  (tag::element_t{}, div_s),
      end(tag::element_t{}, div_s)
    );

    (void)result;
    (void)expected;

    CHECK_TYPE_EQUAL(result, expected);
  }

  // Flatten match spec
  {
    constexpr auto spec = div(
      attr_class("foo"_s),
      match(
        get("key"_s)
      , when<int>(text_node("Hello, int!"_s))
      , when<>(text_node("Hello, everything else!"_s))
      )
    );

    constexpr auto result = hana::type_c<decltype(nbdl::webui::detail::flatten_spec(spec))>;
    constexpr auto expected = hana::type_c<decltype(mpdef::make_list(
      begin(tag::element_t{}, div_s),
        action_fn<tag::attribute_t, decltype("class"_s), mpdef::list<decltype("foo"_s)>>{}
      , action_fn<match_tag, path_t<get_t<decltype("key"_s)>>
        , mpdef::list<
            mpdef::pair<hana::type<int>, mpdef::list<
              action_fn<tag::text_node_t, decltype("Hello, int!"_s)>
            >>
          , mpdef::pair<hana::type<void>, mpdef::list<
              action_fn<tag::text_node_t, decltype("Hello, everything else!"_s)>
            >>
          >
        >{}
    , end(tag::element_t{}, div_s)
    ))>;

    (void)result;
    (void)expected;

    CHECK_TYPE_EQUAL(result, expected);
  }
}
