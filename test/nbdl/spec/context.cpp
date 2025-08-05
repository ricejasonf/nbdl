//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

namespace foo {
heavy_scheme {
  (import (prefix (heavy base) base.))
  (import (heavy clang)
          (heavy mlir)
          (nbdl comp)
          (nbdl spec))
  (context 'my_context (arg1 arg2)
    (member 'foo 'int 42)
    (member 'bar 'std::string
           "initial string value")
    (member 'baz 'int arg1)
    (member 'boo 'std::string
           arg2))
  (translate-cpp
   (module-lookup
    current-nbdl-module
    'my_context))
}
}

int main()
{

}
