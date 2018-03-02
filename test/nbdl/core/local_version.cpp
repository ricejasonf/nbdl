//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/apply_message.hpp>
#include <nbdl/get.hpp>
#include <nbdl/local_version.hpp>
#include <nbdl/match.hpp>
#include <nbdl/uid.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <type_traits>

namespace hana = boost::hana;
namespace message = nbdl::message;

namespace
{
  struct payload { int value; };
  struct fake_uid { int value; };
}

namespace nbdl
{
  NBDL_ENTITY(
    payload
    , value
  );

  NBDL_ENTITY(
    fake_uid
    , value
  );
}

TEST_CASE("Changes are applied immediately and not reverted in any intermediate state when confirmed"
        , "[local_version]")
{
  nbdl::local_version<payload> store{};
  auto gen_uid = nbdl::make_uid_generator();

  auto request = [&](payload p)
  {
    auto m = message::make_upstream_update(
      hana::make_tuple() // path
    , gen_uid()
    , nbdl::make_delta<nbdl::delta_tag>(p)
    );

    nbdl::apply_message(store, m);
    return m;
  };

  auto confirm = [&](auto m)
  {
    nbdl::apply_message(store, message::to_downstream_confirmed(m));
  };

  auto m1 = request(payload{1});
  CHECK(nbdl::get(store).value == 1);

  auto m2 = request(payload{2});
  CHECK(nbdl::get(store).value == 2);

  confirm(m1);
  CHECK(nbdl::get(store).value == 2); // does not revert to 1

  auto m3 = request(payload{3});
  CHECK(nbdl::get(store).value == 3);

  confirm(m2);
  CHECK(nbdl::get(store).value == 3); // does not revert to 2

  auto m4 = request(payload{4});
  CHECK(nbdl::get(store).value == 4);

  auto m5 = request(payload{5});
  CHECK(nbdl::get(store).value == 5);

  confirm(m3);
  CHECK(nbdl::get(store).value == 5); // does not revert to 3

  confirm(m4);
  CHECK(nbdl::get(store).value == 5); // does not revert to 4

  confirm(m5);
  CHECK(nbdl::get(store).value == 5);
}
