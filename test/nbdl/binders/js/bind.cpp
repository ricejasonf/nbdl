//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/binder/js.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/match.hpp>
#include <nbdl/string.hpp>
#include <nbdl/variant.hpp>
#include <nbdl/util/base64_decode.hpp>
#include <nbdl/util/base64_encode.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/pair.hpp>
#include <catch.hpp>
#include <emscripten.h>
#include <vector>

namespace hana = boost::hana;

namespace
{
  struct account
  {
    nbdl::string name_first;
    nbdl::string name_last;
    int age;
    std::vector<hana::pair<unsigned, nbdl::string>> some_list;
    std::vector<unsigned char> buf;
  };
}

namespace nbdl
{
  NBDL_ENTITY(account, name_first, name_last, age, some_list, buf);
}

TEST_CASE("Entity can read and write to and from js_val.", "[binder][js]") 
{
  using nbdl::binder::js::bind_from;
  using nbdl::binder::js::bind_to;

  nbdl::detail::js_val::init_registry();

  nbdl::optional<account> subject1{};
  nbdl::optional<account> subject2{};
  nbdl::binder::js::js_val input_val, output_val;

  EM_ASM_(
    {
      Module.NBDL_DETAIL_JS_SET($0, [1, {
        name_first: "Skippy"
      , name_last: "McGee"
      , age: 9
      , some_list: [
          [100, "foo"]
        , [101, "bar"]
        , [102, "baz"]
        ]
      , buf: "cGxlYXN1cmUu"
      }])
    }
  , input_val.handle()
  );

  bind_from(input_val, subject1);
  bind_to(output_val, subject1);
  bind_from(output_val, subject2);

  nbdl::match(subject2, nbdl::match_when<account>([](auto const& x)
  {
    CHECK(x.name_first == "Skippy");
    CHECK(x.name_last  == "McGee");
    CHECK(x.age == 9);
    CHECK(x.some_list.size() == 3);
  }));

  CHECK(hana::equal(
    subject2
  , nbdl::optional<account>(account{
      "Skippy"
    , "McGee"
    , 9
    , std::vector<hana::pair<unsigned, nbdl::string>>{
        hana::make_pair(100, nbdl::string{"foo"})
      , hana::make_pair(101, nbdl::string{"bar"})
      , hana::make_pair(102, nbdl::string{"baz"})
      }
    , nbdl::util::base64_decode(std::string("pleasure."))
    })
  ));
}
