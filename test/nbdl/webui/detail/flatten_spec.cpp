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
namespace hanax = boost::hana::experimental;
using namespace hana::literals;

int main()
{
  using namespace nbdl::webui::html;
  constexpr auto div_tag    = hana::type_c<hana::string<'d', 'i', 'v'>>;
  constexpr auto begin      = hana::partial(
    hana::template_<hanax::types>,
    hana::type_c<nbdl::webui::detail::begin>
  );
  constexpr auto end        = hana::partial(
    hana::template_<hanax::types>,
    hana::type_c<nbdl::webui::detail::end>
  );
  constexpr auto make_list  = hana::template_<mpdef::list>;

  {
    constexpr auto spec = div();

    constexpr auto result = hana::typeid_(nbdl::webui::detail::flatten_spec(spec));
    constexpr auto expected = make_list(
      begin(tag::element, div_tag),
      end  (tag::element, div_tag)
    );
    (void)result;
    (void)expected;

    BOOST_HANA_CONSTANT_CHECK(hana::equal(result, expected));
  }
  {
    constexpr auto spec = div(div(), div());

    constexpr auto result = hana::typeid_(nbdl::webui::detail::flatten_spec(spec));
    constexpr auto expected = make_list(
      begin(tag::element, div_tag),
        begin(tag::element, div_tag),
        end  (tag::element, div_tag),
        begin(tag::element, div_tag),
        end  (tag::element, div_tag),
      end  (tag::element, div_tag)
    );
    (void)result;
    (void)expected;

    BOOST_HANA_CONSTANT_CHECK(hana::equal(result, expected));
  }
  {
    constexpr auto spec = div(
      attr_class("foo"_s),
      div(
        text_content("Hello World!"_s)
      )
    );

    constexpr auto result = hana::typeid_(nbdl::webui::detail::flatten_spec(spec));
    constexpr auto expected = make_list(
      begin(tag::element, div_tag),
        hana::type<hanax::types<tag::attribute_t, decltype("class"_s), decltype("foo"_s)>>{},
        begin(tag::element, div_tag),
          hana::typeid_(hanax::types<tag::text_content_t, decltype("Hello World!"_s)>{}),
        end  (tag::element, div_tag),
      end(tag::element, div_tag)
    );
    (void)result;
    (void)expected;

    //using foo = typename decltype(result)::foo;

    BOOST_HANA_CONSTANT_CHECK(hana::equal(result, expected));
  }
}
