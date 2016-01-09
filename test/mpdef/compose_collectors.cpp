//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<mpdef/ComposeCollectors.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

template<int i>
constexpr auto counter = hana::demux(hana::partial(hana::plus, hana::int_c<i>))(hana::arg<1>);

int main()
{
  {
    constexpr auto xs = hana::to_tuple(hana::range_c<int, 0, 5>);
    constexpr auto initial_state = hana::make_tuple(
      hana::int_c<0>,
      hana::int_c<0>,
      hana::int_c<0>
    );
    constexpr auto collector = mpdef::composeCollectors(
      counter<1>,
      counter<2>,
      counter<3>
    );
    constexpr auto result = collector(initial_state, xs);

    BOOST_HANA_CONSTANT_ASSERT(result == hana::tuple_c<int, 1, 2, 3>);
  }

  {
    constexpr auto xs = hana::to_tuple(hana::range_c<int, 0, 5>);

    constexpr auto result = hana::fold(xs,
      hana::make_tuple(
        hana::int_c<0>,
        hana::int_c<0>,
        hana::int_c<0>,
        hana::make_tuple()
      ),
      mpdef::composeCollectors(
        counter<1>,
        counter<2>,
        counter<3>,
        hana::append
      ));
    constexpr auto expected = hana::make_tuple(
      hana::int_c<5>,
      hana::int_c<10>,
      hana::int_c<15>,
      xs
    );
    BOOST_HANA_CONSTANT_ASSERT(result == expected);
  }
}
