//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <nbdl/hold.hpp>
#include <nbdl/hold_lazy.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/run_async.hpp>

#include <boost/hana/tuple.hpp>
#include <catch.hpp>
#include <functional>

namespace hana = boost::hana;

namespace
{
  template <typename T>
  struct ptr_wrapper
  {
    T* ptr;
  };

  struct non_copyable
  {
    non_copyable(int x, ptr_wrapper<non_copyable>& self, std::function<void(non_copyable*)> resolver)
      : value(x)
      , resolver(resolver)
    {
      self.ptr = this;
    }

    non_copyable(non_copyable const&) = delete;
    
    void resolve()
    {
      resolver(this);
    }

    int value;
    std::function<void(non_copyable*)> resolver;
  };
}

TEST_CASE("Hold a lazily constructed value in a promise", "[hold][hold_lazy]")
{
  int result = 0;

  ptr_wrapper<non_copyable> inst{};

  nbdl::run_async(hana::make_tuple(
    nbdl::hold(42)
  , nbdl::hold_lazy(
      [&](auto& resolver, int& value)
      {
        return non_copyable(value, inst, [&resolver](non_copyable* self) { resolver(*self); });
      }
    , nbdl::lazy_args<int&>
    )
  , [&](non_copyable& held)
    {
      result = held.value;
    }
  ));

  // Simulate an async call
  inst.ptr->resolve();

  CHECK(result == 42);
}
