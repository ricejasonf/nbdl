//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/catch.hpp>
#include <nbdl/pipe.hpp>
#include <nbdl/run_async.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <functional>

namespace hana = boost::hana;

namespace
{
  template <int i>
  auto make_op()
  {
    return nbdl::promise([](auto&& resolver, auto&& state)
    {
      resolver.resolve(hana::append(state, i));
    });
  }

  template <int i>
  auto op = make_op<i>();

  auto tap = [](auto fn)
  {
    return nbdl::promise([fn_ = std::move(fn)](auto&& resolve, auto&& state)
    {
      using State = decltype(state);
      fn_(state);
      resolve(std::forward<State>(state));
    });
  };

  template<typename Fn>
  struct check_t 
  {
    Fn fn;
    int check_count_;

    template<typename Arg>
    decltype(auto) operator()(Arg&& arg)
    {
      fn(arg);
      ++check_count_;
      return std::forward<Arg>(arg);
    }

    bool get_check_count() { return check_count_; }
  };

  template <typename Fn>
  auto make_check(Fn&& fn) { return check_t<Fn>{std::forward<Fn>(fn), 0}; }
}

TEST_CASE("Pipe asynchronous events", "[pipe]")
{
  auto check_1 = make_check(
    [](auto const& state) { CHECK(hana::equal(state, hana::make_tuple(42, 1))); }
  );

  auto check_2 = make_check(
    [](auto const& state) { CHECK(hana::equal(state, hana::make_tuple(42, 1, 2, 3, 4))); }
  );

  nbdl::run_async(
    nbdl::pipe(
      op< 1 >
    , [&](auto const& state) noexcept { check_1(state); return state; }
    , op< 2 >
    , op< 3 >
    , op< 4 >
    , [&](auto const& state) noexcept { check_2(state); return state; }
    )
  , hana::make_tuple(42)
  );

  CHECK(check_1.get_check_count() == 1);
  CHECK(check_2.get_check_count() == 1);
}

TEST_CASE("Pipes are nestable and allow synchronous transformations", "[pipe]")
{
  auto check_1 = make_check(
    [](auto const& state) { CHECK(hana::equal(state, hana::make_tuple(1, 4, 9, 16, 25, 36, 49))); }
  );

  auto pipe_1 = nbdl::pipe(
    op< 1 >
  , op< 2 >
  , op< 3 >
  , op< 4 >
  );

  nbdl::run_async(
    nbdl::pipe(
      pipe_1
    , nbdl::pipe(
        op< 5 >
      , op< 6 >
      , op< 7 >
      )
    , [](auto&& xs) noexcept
      {
        return hana::transform(
          std::forward<decltype(xs)>(xs)
        , [](auto x) { return x * x; }
        );
      }
    , [&](auto const& state) noexcept { check_1(state); return state; }
    )
  , hana::make_tuple()
  );
}

TEST_CASE("Pipe with a rejection", "[pipe]")
{
  int check_count = 0;

  nbdl::run_async(
    nbdl::pipe(
      op< 1 >
    , nbdl::promise([](auto&& resolve, auto&&) { resolve.reject(hana::int_c<1>); })
    , tap([](auto const&) { CHECK(false); })
    , op< 3 >
    , tap([](auto const&) { CHECK(false); })
    , op< 4 >
    , nbdl::catch_([&](hana::int_<1>) { check_count++; })
    , nbdl::catch_([&](hana::int_<2>) { CHECK(false); })
    )
  , hana::make_tuple(42)
  );

  CHECK(check_count == 1);
}

TEST_CASE("Pipe with a rejection with a skipped catch", "[pipe]")
{
  int check_count = 0;

  nbdl::run_async(
    nbdl::pipe(
      op< 1 >
    , nbdl::promise([](auto&& resolve, auto&&) { resolve.reject(hana::int_c<2>); })
    , tap([](auto const&) { CHECK(false); })
    , op< 3 >
    , tap([](auto const&) { CHECK(false); })
    , op< 4 >
    , nbdl::catch_([&](hana::int_<1>) { CHECK(false); })
    , nbdl::catch_([&](hana::int_<2>) { check_count++; })
    )
  , hana::make_tuple(42)
  );

  CHECK(check_count == 1);
}
