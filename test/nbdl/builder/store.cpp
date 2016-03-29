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
#include <nbdl/entity_members.hpp>
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
  struct e1
  {
    int x;
  };
}

namespace nbdl {
  NBDL_ENTITY(entity::e1, x);
}

int main()
{
  {
    using builder::access_point_meta;
    constexpr auto store_impl = hana::template_<nbdl::store_container::hash_map>;
    constexpr auto path_type = nbdl::path_type<int, entity::e1>;
    constexpr auto access_point = builder::make_access_point_meta_with_map(
      access_point_meta::name           = names::Foo,
      access_point_meta::actions        = hana::make_tuple(nbdl_def::tag::Create),
      access_point_meta::store_container = store_impl,
      access_point_meta::entity_names    = hana::make_tuple(names::E1)
    );

    BOOST_HANA_CONSTANT_ASSERT(
      builder::store(path_type, access_point)
        ==
      hana::template_<nbdl::store>(
        store_impl(path_type),
        path_type
      )
    );
  }
}
