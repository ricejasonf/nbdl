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
  (context 'my_context (member) (arg1 arg2)
    (member 'foo 'int 42)
    (member 'bar 'std::string
           "initial string value")
    (member 'baz 'int arg1)
    (member 'boo 'std::string
           arg2))
}
}

TEST_CASE("Construct context", "[spec][context]") {
  std::string boo = "this is a boo";
  auto ctx = foo::my_context(5, std::move(boo));
  CHECK(ctx.get_foo() == 42);
  CHECK(ctx.get_bar() == std::string("initial string value"));
  CHECK(ctx.get_baz() == 5);
  CHECK(ctx.get_boo() == std::string("this is a boo"));
}
