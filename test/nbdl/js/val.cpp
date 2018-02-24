//
// Copyright Jason Rice 2018
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/js.hpp>

#include <boost/hana/equal.hpp>
#include <catch.hpp>

namespace hana = boost::hana;

TEST_CASE("Transform a nbdl::js::val.", "[js]")
{
  nbdl::js::init();

  nbdl::js::val val;
  nbdl::js::val expected;

  NBDL_JS_TRANSFORM(val, function(x) { return 5; });
  NBDL_JS_TRANSFORM(val, function(x) { return x * x; });

  NBDL_JS_TRANSFORM(expected, function(x) { return 25; });

  CHECK(hana::equal(val, expected));
}

TEST_CASE("Copy val.", "[js]")
{
  nbdl::js::init();

  nbdl::js::val val1;
  nbdl::js::val val2;
  nbdl::js::val expected1;
  nbdl::js::val expected2;

  NBDL_JS_TRANSFORM(val1, function(x) { return 42; });
  val2 = val1;
  NBDL_JS_TRANSFORM(val2, function(x) { return x + x; });

  NBDL_JS_TRANSFORM(expected1, function(x) { return 42; });
  NBDL_JS_TRANSFORM(expected2, function(x) { return 84; });

  CHECK(hana::equal(val1, expected1));
  CHECK(hana::equal(val2, expected2));
}

TEST_CASE("Move val should unregister js value before delete.", "[js]")
{
  nbdl::js::init();

  nbdl::js::val val1;
  nbdl::js::val val2;
  nbdl::js::val expected1;
  nbdl::js::val expected2;

  NBDL_JS_TRANSFORM(val1, function(x) { return 42; });
  val2 = std::move(val1);
  NBDL_JS_TRANSFORM(val2, function(x) { return x + x; });

  NBDL_JS_TRANSFORM(expected1, function(x) { return undefined; });
  NBDL_JS_TRANSFORM(expected2, function(x) { return 84; });

  CHECK(hana::equal(val1, expected1));
  CHECK(hana::equal(val2, expected2));
}
