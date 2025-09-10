//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

namespace foo {
heavy_scheme {
  (import (heavy clang))
  (write-lexer "
    struct woof {
      static constexpr bool is_woof = true;
    };"
  )
}
}

int main()
{
  static_assert(foo::woof::is_woof, "Expect woof.");
}
