//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/hana/equal.hpp>
#include <catch.hpp>
#include <nbdl/concept/Buffer.hpp>
#include <nbdl/uid.hpp>

#include <unordered_set>

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

static_assert(nbdl::Buffer<nbdl::uid>, "nbdl::uid is a nbdl::Buffer");

TEST_CASE("Smoke test uid generation", "[core]")
{
  auto gen = nbdl::make_uid_generator();

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

TEST_CASE("nbdl::uid is Hashable", "[core]")
{
  auto gen = nbdl::make_uid_generator();

  {
    // DefaultConstructible
    nbdl::uid x{};
    nbdl::uid y{};

    // CopyAssignable
    x = gen();
    y = gen();

    // Swappable
    std::swap(x, y);

    // Descructible
    x.~uid();

    // Hashable
    std::hash<nbdl::uid>{}(x);
  }

  std::unordered_set<nbdl::uid> set;
  set.insert(gen());
  set.insert(gen());
  set.insert(gen());
  set.insert(gen());
  CHECK(set.size() == 4);
}
