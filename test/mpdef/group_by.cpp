//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/group_by.hpp>
#include <mpdef/pair.hpp>

#include <type_traits>

namespace hana = boost::hana;

namespace
{
  template <int>
  struct key { };

  template <int, int>
  struct value { };
}

int main()
{
  using L = mpdef::list<
    mpdef::list<key<0>, mpdef::list<value<0, 0>, value<0, 1>>>
  , mpdef::list<key<1>, mpdef::list<>>
  , mpdef::list<key<0>, mpdef::list<>>
  , mpdef::list<key<1>, mpdef::list<value<1, 0>>>
  , mpdef::list<key<1>, mpdef::list<value<1, 1>>>
  , mpdef::list<key<0>, mpdef::list<value<0, 2>>>
  , mpdef::list<key<2>, mpdef::list<>>
  >;

  using Expected = mpdef::map<
    mpdef::pair<key<0>, mpdef::list<value<0, 0>, value<0, 1>, value<0, 2>>>
  , mpdef::pair<key<1>, mpdef::list<value<1, 0>, value<1, 1>>>
  , mpdef::pair<key<2>, mpdef::list<>>
  >;

  using Result = mpdef::group_by<L>;

  static_assert(
    std::is_same<Result, Expected>::value
  , "Make an mpdef::map by grouping a list of pairs by their keys"
  );
}
