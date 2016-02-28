//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/AccessPointMeta.hpp>
#include<def/builder/EntityMeta.hpp>
#include<def/builder/Path.hpp>
#include<def/builder/Store.hpp>
#include<def/directives.hpp>
#include<EntityTraits.hpp>
#include<Path.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;
namespace builder = nbdl_def::builder;

#define DEFINE_TYPE(NAME) \
  struct NAME##_t {}; constexpr auto NAME = hana::type_c<NAME##_t>;

namespace names {
  DEFINE_TYPE(Foo);
  DEFINE_TYPE(E1);
}//names

namespace entity {
  struct E1
  {
    int x;
  };
}

namespace nbdl {
  NBDL_ENTITY(entity::E1, x);
}

int main()
{
  {
    using builder::AccessPointMeta;
    constexpr auto store_impl = hana::template_<nbdl::store::HashMap>;
    constexpr auto path_type = nbdl::path_type<int, entity::E1>;
    constexpr auto access_point = builder::makeAccessPointMetaWithMap(
      AccessPointMeta::name           = names::Foo,
      AccessPointMeta::actions        = hana::make_tuple(nbdl_def::tag::Create),
      AccessPointMeta::storeContainer = store_impl,
      AccessPointMeta::entityNames    = hana::make_tuple(names::E1)
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::store(path_type, access_point)
        ==
      hana::template_<nbdl::Store>(
        store_impl(path_type),
        path_type
      )
    );
  }
}
