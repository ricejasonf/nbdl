//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/ui_view/detail/flatten_spec.hpp>
#include <nbdl/ui_view/spec.hpp>

#include <boost/hana.hpp>

namespace hana  = boost::hana;
using namespace hana::literals;

#define CHECK_TYPE_EQUAL(RESULT, EXPECTED) BOOST_HANA_CONSTANT_CHECK(hana::equal( \
  hana::typeid_(RESULT), hana::typeid_(EXPECTED) \
));

int main()
{
  using namespace nbdl::ui_spec;
  using namespace nbdl::ui_view::spec;
  using nbdl::ui_view::detail::action_fn;

  auto begin = [](auto ...xs)
  {
    return action_fn<nbdl::ui_view::detail::begin, decltype(xs)...>{};
  };

  auto end = [](auto ...xs)
  {
    return action_fn<nbdl::ui_view::detail::end, decltype(xs)...>{};
  };

  {
    constexpr auto spec = ui_view();

    constexpr auto result = nbdl::ui_view::detail::flatten_spec(spec);
    constexpr auto expected = mpdef::make_list(
      begin(tag::view_t{}, hana::type<UIView>),
      end(tag::view_t{}, hana::type<UIView>)
    );
    (void)result;
    (void)expected;

    CHECK_TYPE_EQUAL(result, expected);
  }
}
