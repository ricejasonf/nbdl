//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <catch.hpp>

namespace foo {
heavy_scheme {
  (import (nbdl spec))
  (context 'context_1 (arg1 arg2)
    (member 'foo 'int 42)
    (member 'bar 'std::string
           "initial string value")
    (member 'baz 'int arg1)
    (member 'boo 'std::string arg2))

  (match-fn 'match_1 (store fn)
    (define root-path (get store))
    (fn
      (get store 'foo)
      (get root-path 'boo)))
}
}

TEST_CASE("Match context members", "[spec][match]") {
  std::string boo = "this is a boo";
  auto ctx = foo::context_1(5, std::move(boo));
  std::string result_baz;
  std::string result_boo;

  match_1(ctx, [&](int baz, std::string const& boo) {
    result_baz = baz;
    result_boo = boo;
  });

  CHECK(result_baz == 5);
  CHECK(result_boo == std::string("this is a boo"));
}
