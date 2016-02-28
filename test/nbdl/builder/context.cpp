//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include<def/builder/Context.hpp>
#include<def/directives.hpp>
#include<macros/NBDL_ENTITY.hpp>
#include<Path.hpp>

#include<boost/hana.hpp>

namespace hana = boost::hana;

struct DummyProvider { };

namespace entity {
  struct Client
  {
    int id;
  };
  struct MyEntity
  {
    int id;
    int client_id;
  };
}//entity

namespace nbdl {
  NBDL_ENTITY(
    entity::Client,
      id);
  NBDL_ENTITY(
    entity::MyEntity,
      id,
      client_id );
}//nbdl
using OnlySupportedPath =
  typename decltype(
    nbdl::path_type<int, entity::Client, entity::MyEntity>
  )::type;


namespace my_context {
  using namespace nbdl_def;

  auto create = buildContextFactory(
    Context(
      Entities(
        Entity(
          Type(hana::type_c<entity::Client>)
        ),
        Entity(
          Type(hana::type_c<entity::MyEntity>)
        )
      ),
      // TODO: Define a Consumer here.
      Provider(
        Type(hana::type_c<DummyProvider>),
        AccessPoint(
          Name(hana::type_c<void>),
          EntityName(hana::type_c<entity::Client>),
          Actions(Read()),
          AccessPoint(
            Name(hana::type_c<void>),
            EntityName(hana::type_c<entity::MyEntity>),
            Actions(Read())
          )
        )
      )
    )
  );
} // my_context

int main()
{
  // Okay, We're not actually testing anything here, but
  // at least we'll know this compiles, and a better test
  // can be added once the representation of Context is
  // closer to being sorted out.
  auto ctx = my_context::create();
}
