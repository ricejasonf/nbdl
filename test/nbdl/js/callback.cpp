//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/js.hpp>

#include <catch.hpp>
#include <emscripten.h>

TEST_CASE("Create a callback and invoke it in javascript land.", "[js]")
{
  constexpr auto get_handle = nbdl::js::detail::get_handle;
  nbdl::js::init();

  bool result = false;

  auto cb = nbdl::js::callback([&](nbdl::js::val val)
  {
    result = EM_ASM_INT(
      {
        return Module.NBDL_DETAIL_JS_GET($0) == "moo";
      }
    , get_handle(val)
    );
  });

  EM_ASM_(
    {
      Module.NBDL_DETAIL_JS_GET($0)("moo");
    }
  , get_handle(cb)
  );

  CHECK(result);
}
