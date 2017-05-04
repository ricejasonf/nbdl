//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mpdef/list.hpp>
#include <nbdl/params_promise.hpp>
#include <nbdl/pipe.hpp>
#include <nbdl/run_sync.hpp>
#include <nbdl/tap.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/map.hpp>
#include <catch.hpp>
#include <string>

namespace hana = boost::hana;

namespace
{
  struct foo_t { };
  constexpr hana::type<foo_t> foo{};
}

TEST_CASE("params_promise resolves parameters which may be path specs.", "[params_promise][Store]")
{
  using nbdl::ui_spec::get;

  auto store = hana::make_map(
    hana::make_pair(foo, std::string("OK!"))
  );

  std::string result("Fail!");

  nbdl::run_sync(
    nbdl::pipe(
      nbdl::params_promise(mpdef::make_list(hana::int_c<0>, get(foo), hana::int_c<2>))
    , nbdl::tap([&](auto const& tuple) { 
        static_assert(decltype(hana::equal(hana::int_c<0>, hana::at_c<0>(tuple))){});
        static_assert(decltype(hana::equal(hana::int_c<2>, hana::at_c<2>(tuple))){});
        static_assert(decltype(hana::equal(
          hana::type_c<std::reference_wrapper<const std::string>>
        , hana::typeid_(hana::at_c<1>(tuple))
        )){});
        result = hana::at_c<1>(tuple);
      })
    )
  , store
  );

  CHECK(result == std::string("OK!"));
}
