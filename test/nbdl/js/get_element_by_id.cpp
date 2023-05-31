//
// Copyright Jason Rice 2019
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/js.hpp>

#include <boost/hana/equal.hpp>
#include <catch.hpp>

namespace hana = boost::hana;
using namespace boost::hana::literals;

TEST_CASE("get_element_by_id from document object.", "[js]")
{
  nbdl::js::init();
  EM_ASM({
    // set up fake document object
    document = {
      getElementById : function(id) {
        if (id == 'foo') return 1;
        else if (id == 'bar') return 2;
        else return 0;
      }
    }
  });

  nbdl::js::val foo = nbdl::js::get_element_by_id("foo"_s);
  nbdl::js::val bar = nbdl::js::get_element_by_id("bar"_s);

  nbdl::js::val foo_expected;
  nbdl::js::val bar_expected;
  NBDL_JS_TRANSFORM(foo_expected, function(x) { return 1; });
  NBDL_JS_TRANSFORM(bar_expected, function(x) { return 2; });

  CHECK(hana::equal(foo, foo_expected));
  CHECK(hana::equal(bar, bar_expected));
}
