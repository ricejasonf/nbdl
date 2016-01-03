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

NBDL_DEF_DIRECTIVE(FindMe);
NBDL_DEF_DIRECTIVE(Root);
NBDL_DEF_DIRECTIVE(A);
NBDL_DEF_DIRECTIVE(B);

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
    constexpr auto foldFn = hana::always(hana::tuple<>{});
    constexpr auto x = nbdl_def_meta::createInTreeFinder(pred, foldFn)(def);
    constexpr auto y = hana::make_tuple(
      hana::make_pair(FindMe(A()) , hana::make_tuple()),
      hana::make_pair(FindMe()    , hana::make_tuple()),
      hana::make_pair(FindMe()    , hana::make_tuple()),
      hana::make_pair(FindMe(B()) , hana::make_tuple()),
      hana::make_pair(FindMe()    , hana::make_tuple()),
      hana::make_pair(FindMe(B()) , hana::make_tuple())
    );
    //std::cout << hana::experimental::print(x) << ";\n";
    //std::cout << "\n";
    //std::cout << hana::experimental::print(y) << "\n";
    BOOST_HANA_CONSTANT_ASSERT(x == y);
  }
}
