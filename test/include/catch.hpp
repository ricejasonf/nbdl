//
// Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved.
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_TEST_CATCH_HPP
#define NBDL_TEST_CATCH_HPP

#ifdef NBDL_USE_CATCH
#include <catch_orig.hpp>
#else

#include <cstdlib>
#include <vector>

// Custom shim for Catch Test Framework

extern std::vector<void (*)()> catch_shim_test_cases;

struct catch_shim_test_case
{
  template <typename Fn>
  catch_shim_test_case(Fn fn)
  {
    catch_shim_test_cases.push_back(fn);
  }
};

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )
#define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )

#define TEST_CASE( ... ) \
    static void INTERNAL_CATCH_UNIQUE_NAME(CATCH_SHIM_TEST)(); \
    namespace{ catch_shim_test_case INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(CATCH_SHIM_TEST)); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(CATCH_SHIM_TEST)()

#define REQUIRE(cond) if (!(cond)) std::exit(1);
#define CHECK(cond)   if (!(cond)) std::exit(1);

#endif

#endif
