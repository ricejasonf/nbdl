//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/AccessPointMeta.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;


int main()
{
  constexpr auto name         = hana::int_c<0>;
  constexpr auto actions      = hana::int_c<1>;
  constexpr auto store        = hana::int_c<2>;
  constexpr auto storeEmitter = hana::int_c<3>;
  constexpr auto entityNames  = hana::int_c<4>;

  constexpr auto x = nbdl_def::builder::makeAccessPointMeta(
    name
    , actions
    , store
    , storeEmitter
    , entityNames
  );
  BOOST_HANA_CONSTANT_ASSERT(x.name() == name);
  BOOST_HANA_CONSTANT_ASSERT(x.actions() == actions);
  BOOST_HANA_CONSTANT_ASSERT(x.store() == store);
  BOOST_HANA_CONSTANT_ASSERT(x.storeEmitter() == storeEmitter);
  BOOST_HANA_CONSTANT_ASSERT(x.entityNames() == entityNames);
}
