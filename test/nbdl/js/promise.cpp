//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/catch.hpp>
#include <nbdl/js.hpp>
#include <nbdl/run_async.hpp>

#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <emscripten/em_asm.h>

namespace hana = boost::hana;

TEST_CASE("Create a promise to be RESOLVED in javascript land.", "[js]")
{
  constexpr auto get_handle = nbdl::js::detail::get_handle;
  nbdl::js::init();

  bool result = false;

  nbdl::run_async(hana::make_tuple(
    NBDL_JS_PROMISE(function(resolve, reject)
    {
      // this is javascript
      resolve("moo");
    })
  , [&](nbdl::js::val val)
    {
      result = EM_ASM_INT(
        {
          return Module.NBDL_DETAIL_JS_GET($0) == "moo";
        }
      , get_handle(val)
      );
    }
  , nbdl::catch_([](auto&&){})
  ));

  CHECK(result);
}

TEST_CASE("Create a promise to be REJECTED in javascript land.", "[js]")
{
  constexpr auto get_handle = nbdl::js::detail::get_handle;
  nbdl::js::init();

  bool result = false;

  nbdl::run_async(hana::make_tuple(
    NBDL_JS_PROMISE(function(resolve, reject)
    {
      // this is javascript
      reject("moo");
    })
  , nbdl::catch_([&](auto const& val)
    {
      result = EM_ASM_INT(
        {
          return Module.NBDL_DETAIL_JS_GET($0) == "moo";
        }
      , get_handle(val)
      );
    })
  ));

  CHECK(result);
}

TEST_CASE("Create a promise capturing values for input.", "[js]")
{
  constexpr auto get_handle = nbdl::js::detail::get_handle;
  nbdl::js::init();

  bool result = false;

  nbdl::js::val input0{}
              , input1{}
              , input2{}
              , input3{}
              , input4{}
              , input5{}
              , input6{}
              , input7{}
              ;

  EM_ASM_(
    {
      Module.NBDL_DETAIL_JS_SET($0, "value0");
      Module.NBDL_DETAIL_JS_SET($1, "value1");
      Module.NBDL_DETAIL_JS_SET($2, "value2");
      Module.NBDL_DETAIL_JS_SET($3, "value3");
      Module.NBDL_DETAIL_JS_SET($4, "value4");
      Module.NBDL_DETAIL_JS_SET($5, "value5");
      Module.NBDL_DETAIL_JS_SET($6, "value6");
      Module.NBDL_DETAIL_JS_SET($7, "value7");
    }
  , nbdl::js::detail::get_handle(input0)
  , nbdl::js::detail::get_handle(input1)
  , nbdl::js::detail::get_handle(input2)
  , nbdl::js::detail::get_handle(input3)
  , nbdl::js::detail::get_handle(input4)
  , nbdl::js::detail::get_handle(input5)
  , nbdl::js::detail::get_handle(input6)
  , nbdl::js::detail::get_handle(input7)
  );

  nbdl::run_async(hana::make_tuple(
    NBDL_JS_PROMISE_WITH_CAPTURE(
      function(resolve)
      {
        // this is javascript
        resolve(input0 + input1 + input2 + input3 + input4 + input5 + input6 + input7);
      }
    , input0
    , input1
    , input2
    , input3
    , input4
    , input5
    , input6
    , input7
    )
  , [&](nbdl::js::val val)
    {
      result = EM_ASM_INT(
        {
          return Module.NBDL_DETAIL_JS_GET($0) ==
            "value0value1value2value3value4value5value6value7";
        }
      , get_handle(val)
      );
    }
  , nbdl::catch_([](auto&&){})
  ));

  CHECK(result);
}
