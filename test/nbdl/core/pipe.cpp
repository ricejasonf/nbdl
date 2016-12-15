//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/pipe.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <functional>

namespace hana = boost::hana;

namespace
{
  template <int i>
  auto make_op()
  {
    return nbdl::promise([](auto&& resolve, auto&& /* reject */, auto&& state)
    {
      resolve(hana::append(state, i));
    });
  }

  template <int i>
  auto op = make_op<i>();

  auto tap = [](auto fn)
  {
    return nbdl::promise([fn_ = std::move(fn)](auto&& resolve, auto&&, auto&& state)
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
  // Note that check_1 and check_2 have state so they are passed in
  // as references but this is not a best practice when dealing with
  // asynchronous operations.

  auto check_1 = make_check(
    [](auto const& state) { CHECK(hana::equal(state, hana::make_tuple(42, 1))); }
  );

  auto check_2 = make_check(
    [](auto const& state) { CHECK(hana::equal(state, hana::make_tuple(42, 1, 2, 3, 4))); }
  );

  auto pipe = nbdl::pipe(
    op< 1 >
  , std::ref(check_1)
  , op< 2 >
  , op< 3 >
  , op< 4 >
  , std::ref(check_2)
  )(hana::make_tuple(42));
  (void)pipe;

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

  nbdl::pipe(
    pipe_1
  , nbdl::pipe(
      op< 5 >
    , op< 6 >
    , op< 7 >
    )
  , hana::reverse_partial(hana::transform, [](auto x) { return x * x; })
  , std::ref(check_1)
  )(hana::make_tuple());
}

TEST_CASE("Pipe with a rejection", "[pipe]")
{
  int check_count = 0;

  auto pipe = nbdl::pipe(
    op< 1 >
  , nbdl::promise([](auto&&, auto&& reject, auto&&) { reject(hana::int_c<1>); })
  , tap([](auto const&) { CHECK(false); })
  , op< 3 >
  , tap([](auto const&) { CHECK(false); })
  , op< 4 >
  , nbdl::catch_([&](hana::int_<1>) { check_count++; })
  , nbdl::catch_([&](hana::int_<2>) { CHECK(false); })
  )(hana::make_tuple(42));
  (void)pipe;

  CHECK(check_count == 1);
}

TEST_CASE("Pipe with a rejection with a skipped catch", "[pipe]")
{
  int check_count = 0;

  auto pipe = nbdl::pipe(
    op< 1 >
  , nbdl::promise([](auto&&, auto&& reject, auto&&) { reject(hana::int_c<2>); })
  , tap([](auto const&) { CHECK(false); })
  , op< 3 >
  , tap([](auto const&) { CHECK(false); })
  , op< 4 >
  , nbdl::catch_([&](hana::int_<1>) { CHECK(false); })
  , nbdl::catch_([&](hana::int_<2>) { check_count++; })
  )(hana::make_tuple(42));
  (void)pipe;

  CHECK(check_count == 1);
}
