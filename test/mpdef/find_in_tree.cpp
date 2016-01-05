//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<boost/hana.hpp>
#include<iostream>
#include<boost/hana/experimental/printable.hpp>
#include<mpdef/FindInTree.hpp>

namespace hana = boost::hana;

MPDEF_DIRECTIVE(FindMe);
MPDEF_DIRECTIVE(Root);
MPDEF_DIRECTIVE(A);
MPDEF_DIRECTIVE(B);

constexpr auto countDepth = hana::demux(hana::partial(hana::plus, hana::int_c<1>))(hana::arg<1>);

constexpr auto def =
  Root(
    FindMe(A()),
    A(
      FindMe(),
      B(FindMe()),
      FindMe(B())
    ),
    FindMe(),
    FindMe(B())
  );

int main()
{
  {
    constexpr auto pred = hana::equal.to(tag::FindMe) ^hana::on^ hana::first;
    constexpr auto x = nbdl_def_meta::createInTreeFinder(pred, countDepth, hana::int_c<0>)(def);
    constexpr auto y = hana::make_tuple(
      hana::make_pair(FindMe(A()) , hana::int_c<1>),
      hana::make_pair(FindMe()    , hana::int_c<2>),
      hana::make_pair(FindMe()    , hana::int_c<3>),
      hana::make_pair(FindMe(B()) , hana::int_c<2>),
      hana::make_pair(FindMe()    , hana::int_c<1>),
      hana::make_pair(FindMe(B()) , hana::int_c<1>)
    );
    BOOST_HANA_CONSTANT_ASSERT(x == y);
    std::cout << hana::experimental::print(x) << ";\n";
  }
}
