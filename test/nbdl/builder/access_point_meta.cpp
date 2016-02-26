//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/AccessPointMeta.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

int main()
{
  using builder::AccessPointMeta;
  constexpr auto name           = hana::int_c<0>;
  constexpr auto actions        = hana::int_c<1>;
  constexpr auto storeContainer = hana::int_c<2>;
  constexpr auto storeEmitter   = hana::int_c<3>;
  constexpr auto entityNames    = hana::int_c<4>;

  constexpr auto x = builder::makeAccessPointMeta(
    name
    , actions
    , storeContainer
    , storeEmitter
    , entityNames
  );
  BOOST_HANA_CONSTANT_ASSERT(AccessPointMeta::name(x) == name);
  BOOST_HANA_CONSTANT_ASSERT(AccessPointMeta::actions(x) == actions);
  BOOST_HANA_CONSTANT_ASSERT(AccessPointMeta::storeContainer(x) == storeContainer);
  BOOST_HANA_CONSTANT_ASSERT(AccessPointMeta::storeEmitter(x) == storeEmitter);
  BOOST_HANA_CONSTANT_ASSERT(AccessPointMeta::entityNames(x) == entityNames);

  BOOST_HANA_CONSTANT_ASSERT(hana::equal(x,
    builder::makeAccessPointMetaWithMap(
      AccessPointMeta::name = name,
      AccessPointMeta::actions = actions,
      AccessPointMeta::storeContainer = storeContainer,
      AccessPointMeta::storeEmitter = storeEmitter,
      AccessPointMeta::entityNames = entityNames
    )
  ));
  BOOST_HANA_CONSTANT_ASSERT(hana::equal(x,
    builder::makeAccessPointMetaWithMap(
      AccessPointMeta::actions = actions,
      AccessPointMeta::name = name,
      AccessPointMeta::storeContainer = storeContainer,
      AccessPointMeta::entityNames = entityNames,
      AccessPointMeta::storeEmitter = storeEmitter
    )
  ));
}
