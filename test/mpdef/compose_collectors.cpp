//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/compose_collectors.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

template<int i>
constexpr auto counter = hana::demux(hana::partial(hana::plus, hana::int_c<i>))(hana::arg<1>);

template<int i>
constexpr auto mult = hana::demux(hana::append)
(
  hana::arg<1>,
  hana::demux(hana::partial(hana::mult, hana::int_c<i>))(hana::arg<2>)
);

int main()
{
  {
    constexpr auto xs = hana::to_tuple(hana::range_c<int, 0, 5>);
    constexpr auto initial_state = hana::make_tuple(
      hana::make_tuple(),
      hana::make_tuple(),
      hana::make_tuple()
    );
    constexpr auto collector = mpdef::compose_collectors(
      mult<1>,
      mult<2>,
      mult<3>
    );
    constexpr auto result = hana::fold(xs, initial_state, collector);
    constexpr auto expected = hana::make_tuple(
      hana::tuple_c<int, 0, 1, 2, 3, 4>,
      hana::tuple_c<int, 0, 2, 4, 6, 8>,
      hana::tuple_c<int, 0, 3, 6, 9, 12>
    );

    BOOST_HANA_CONSTANT_ASSERT(result == expected);
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
      mpdef::compose_collectors(
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
