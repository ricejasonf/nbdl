//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/detail/params_view.hpp>

#include <boost/hana/equal.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/unpack.hpp>
#include <catch.hpp>
#include <string>
#include <functional>

namespace hana = boost::hana;

TEST_CASE("Append empties and reference_wrappers to a params_view", "[detail][params_view]")
{
  constexpr auto append = nbdl::detail::params_view_append;
  int a = 42;
  int b = 1234;
  std::string c{"Hello World"};

  auto my_view =  append(std::cref(c),
                  append(hana::int_c<8>,
                  append(hana::int_c<7>,
                  append(hana::int_c<6>,
                  append(std::cref(b),
                  append(hana::int_c<4>,
                  append(hana::int_c<3>,
                  append(std::cref(a),
                  append(hana::int_c<1>,
                  append(hana::int_c<0>,
                  nbdl::detail::make_params_view()
                  ))))))))));

  bool called = false;

  hana::unpack(my_view, [&](hana::int_<0>
                          , hana::int_<1>
                          , auto ref_a       
                          , hana::int_<3>
                          , hana::int_<4>
                          , auto ref_b           
                          , hana::int_<6>
                          , hana::int_<7>
                          , hana::int_<8>
                          , auto ref_c
                          )
  {
    CHECK(ref_a.get() == a);
    CHECK(ref_b.get() == b);
    CHECK(ref_c.get() == c);
    called = true;
  });

  CHECK(called);
}

TEST_CASE("Concat two params_view", "[detail][params_view]")
{
  constexpr auto append = nbdl::detail::params_view_append;
  int a = 42;
  int b = 1234;
  std::string c{"Hello World"};

  auto my_view_1 =  append(std::cref(c),
                    append(hana::int_c<8>,
                    append(hana::int_c<7>,
                    append(hana::int_c<6>,
                    append(std::cref(b),
                    append(hana::int_c<4>,
                    append(hana::int_c<3>,
                    append(std::cref(a),
                    append(hana::int_c<1>,
                    append(hana::int_c<0>,
                    nbdl::detail::make_params_view()
                    ))))))))));

  std::string d{"concat food"};
  int e = 5678;

  auto my_view_2 =  append(std::cref(e),
                    append(std::cref(d),
                    append(hana::int_c<11>,
                    append(hana::int_c<10>,
                    nbdl::detail::make_params_view()
                    ))));

  auto my_view = nbdl::detail::params_view_concat(my_view_1, my_view_2);

  bool called = false;

  hana::unpack(my_view, [&](hana::int_<0>
                          , hana::int_<1>
                          , auto ref_a       
                          , hana::int_<3>
                          , hana::int_<4>
                          , auto ref_b           
                          , hana::int_<6>
                          , hana::int_<7>
                          , hana::int_<8>
                          , auto ref_c
                          , hana::int_<10>
                          , hana::int_<11>
                          , auto ref_d
                          , auto ref_e
                          )
  {
    CHECK(ref_a.get() == a);
    CHECK(ref_b.get() == b);
    CHECK(ref_c.get() == c);
    CHECK(ref_d.get() == d);
    CHECK(ref_e.get() == e);
    called = true;
  });

  CHECK(called);
}
