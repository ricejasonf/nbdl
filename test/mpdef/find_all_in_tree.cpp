//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<boost/hana.hpp>
#include<def/meta.hpp>

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
    constexpr auto x = createAllInTreeFinder(hana::equal.to(tag::FindMe));
    BOOST_HANA_CONSTANT_ASSERT(
      x == hana::make_tuple(
        FindMe(A()),
        FindMe(),
        FindMe(B()),
        FindMe(),
        FindMe(B())
      );
    );
  }
}
