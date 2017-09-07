//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/hana/equal.hpp>
#include <catch.hpp>
#include <nbdl/uid.hpp>

namespace hana = boost::hana;

# if 0
#include <iostream>
#include <iomanip>
namespace
{
  void print(nbdl::uid const& u)
  {
    for (unsigned short x : u.value)
    {
      std::cout << std::setw(4) << x;
    }
    std::cout << '\n';
  }
}
#else
namespace
{
  void print(nbdl::uid const&) { }
}
#endif

TEST_CASE("Smoke test uid generation", "[uid]")
{
  auto gen = nbdl::detail::make_uid_generator();

  {
    nbdl::uid foo = gen();
    print(foo);
    (void)foo;
  }
  {
    nbdl::uid foo = gen();
    print(foo);
    (void)foo;
  }
  {
    nbdl::uid foo = gen();
    print(foo);
    (void)foo;
  }
  {
    nbdl::uid foo = gen();
    print(foo);
    (void)foo;
  }
  {
    nbdl::uid foo = gen();
    print(foo);
    (void)foo;
  }
  {
    nbdl::uid foo = gen();
    print(foo);
    (void)foo;
  }
  {
    nbdl::uid foo = gen();
    print(foo);
    (void)foo;
  }
  {
    nbdl::uid foo = gen();
    print(foo);

    // TEST COPY
    nbdl::uid bar{foo};
    CHECK(hana::equal(foo, bar));
  }
}
