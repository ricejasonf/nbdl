//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/promise.hpp>
#include <nbdl/run_async_loop.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>

namespace hana = boost::hana;

TEST_CASE("Run a series of async calls in a loop", "[run_async_loop]")
{
  int calls_1 = 0;
  int calls_2 = 0;
  auto counter_1 = nbdl::promise([&calls_1](auto& resolve)
  {
    calls_1++;
    resolve();
  });

  auto counter_2 = nbdl::promise([&calls_2](auto& resolve)
  {
    calls_2++;
    if (calls_2 >= 42)
    {
      resolve.terminate();
    }
    else
    {
      resolve();
    }
  });

  nbdl::run_async_loop(hana::make_tuple(counter_1, counter_2));

  CHECK(calls_1 == 42);
  CHECK(calls_2 == 42);
}
