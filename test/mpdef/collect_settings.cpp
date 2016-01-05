//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<boost/hana.hpp>
#include<mpdef/CollectSettings.hpp>

namespace hana = boost::hana;

constexpr auto tag1 = hana::type_c<hana::int_<1>>;
constexpr auto tag2 = hana::type_c<hana::int_<2>>;
constexpr auto tag3 = hana::type_c<hana::int_<3>>;
constexpr auto tag4 = hana::type_c<hana::int_<4>>;

int main()
{
  {
    using mpdef::collectSettings;
    using mpdef::withSettings;

    constexpr auto xs = hana::make_tuple(
      hana::make_map(
        hana::make_pair(tag1, hana::int_c<42>),
        hana::make_pair(tag2, hana::int_c<256>)
      ),
      hana::make_map(),
      hana::make_map(
        hana::make_pair(tag3, hana::int_c<1024>)
      ),
      hana::make_map(
        hana::make_pair(tag2, hana::int_c<512>)
      )
    );
    
    BOOST_HANA_CONSTANT_ASSERT(
      hana::fold_left(xs, withSettings(tag1, tag2, tag3, tag4), collectSettings)
        ==
      hana::make_map(
        hana::make_pair(tag1, hana::just(hana::int_c<42>)),
        hana::make_pair(tag2, hana::just(hana::int_c<512>)),
        hana::make_pair(tag3, hana::just(hana::int_c<1024>)),
        hana::make_pair(tag4, hana::nothing)
      )
    );
  }
}
