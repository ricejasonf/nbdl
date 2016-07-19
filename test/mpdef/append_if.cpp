//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/append_if.hpp>

#include <boost/hana.hpp>

namespace hana = boost::hana;

int main()
{
  {
    constexpr auto xs = hana::to_tuple(hana::range_c<int, 1, 10>);
    constexpr auto pred = hana::demux(hana::not_)(hana::reverse_partial(hana::mod, hana::int_c<2>));
    constexpr auto result = hana::fold(xs, hana::make_tuple(), mpdef::append_if(pred));
    constexpr auto expected = hana::tuple_c<int, 2, 4, 6, 8>;
    BOOST_HANA_CONSTANT_ASSERT(result == expected);
  }
}
