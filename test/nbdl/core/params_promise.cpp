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
#include <nbdl/ui_helper.hpp>
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

  constexpr hana::string<'n', 'o', 't', 'h', 'i', 'n', 'g'> nothing_s;
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
    , nbdl::tap([&](auto const& params) { 
        hana::unpack(params, [&](hana::int_<0>
                              , auto ref_a
                              , hana::int_<2>
                              )
        {
          result = ref_a;
        });
      })
    )
  , store
  );

  CHECK(result == std::string("OK!"));
}

TEST_CASE("Match values by type in params_promise.", "[params_promise][ui_spec][Store]")
{
  using nbdl::ui_spec::get;
  using nbdl::ui_spec::match;
  using nbdl::ui_spec::when;

  using string_option = nbdl::optional<std::string>;

  auto store = hana::make_map(
    hana::make_pair(foo, string_option{})
  );

  constexpr auto matcher = hana::at_c<0>(
    nbdl::ui_helper::flatten_param_node(match(
      get(foo)
    , when<nbdl::nothing>(nothing_s)
    , when<std::string>(get)
    ))
  );

  std::string result("Fail!");

  auto my_pipe = nbdl::pipe(
    nbdl::params_promise(mpdef::make_list(hana::int_c<0>, matcher, hana::int_c<2>))
  , nbdl::tap([&](auto const& params) {
      hana::unpack(params, [&](hana::int_<0>
                            , auto value
                            , hana::int_<2>
                            )
      {
        if constexpr(hana::is_a<hana::string_tag, decltype(value)>)
        {
          result = std::string(hana::to<char const*>(value));
        }
        else
        {
          result = value;
        }
      });
    })
  );
 
  nbdl::run_sync(my_pipe, store);
  CHECK(result == std::string("nothing"));

  store[foo] = std::string{"something"}; 

  nbdl::run_sync(my_pipe, store);
  CHECK(result == std::string("something"));
}
