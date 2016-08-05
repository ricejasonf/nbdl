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

namespace hana = boost::hana;

int main()
{
  using namespace nbdl::webui::html;
  constexpr auto div_tag    = hana::type_c<hana::string<'d', 'i', 'v'>>;
  constexpr auto class_tag  = hana::type_c<hana::string<'c', 'l', 'a', 's', 's'>>;
  constexpr auto begin      = hana::template_<nbdl::webui::detail::begin_fn>;
  constexpr auto end        = hana::template_<nbdl::webui::detail::end_fn>;
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

    BOOST_HANA_CONSTANT_ASSERT(hana::equal(result, expected));
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

    BOOST_HANA_CONSTANT_ASSERT(hana::equal(result, expected));
  }
  {
    constexpr hana::string<'f', 'o', 'o'> foo_str{};
    constexpr auto spec = div(
      attr_class(foo_str),
      div()
    );

    constexpr auto result = hana::typeid_(nbdl::webui::detail::flatten_spec(spec));
    constexpr auto expected = make_list(
      begin(tag::element, div_tag),
        begin(tag::attribute, class_tag),
        hana::typeid_(foo_str),
        end  (tag::attribute, class_tag),
        begin(tag::element, div_tag),
        end  (tag::element, div_tag),
      end(tag::element, div_tag)
    );
    (void)result;
    (void)expected;

    BOOST_HANA_CONSTANT_ASSERT(hana::equal(result, expected));
  }
}
