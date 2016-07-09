//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifdef NBDL_USE_CATCH
#define CATCH_CONFIG_MAIN
#include <catch_orig.hpp>
#else

#include <catch.hpp>

std::vector<void (*)()> catch_shim_test_cases;
int main()
{
  for (void (*fn)() : catch_shim_test_cases)
  {
    fn();
  }
}
#endif
