//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<nbdl>

#include<boost/hana.hpp>
#include<catch.hpp>
#include<functional>

namespace hana = boost::hana;

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


namespace my_context {
  using namespace nbdl_def;

  template<typename Provider_>
  auto create = nbdl_def::buildContextFactory(
    nbdl_def::Context(
      Entities(
        Entity(
          Type(hana::type_c<entity::Client>)
        ),
        Entity(
          Type(hana::type_c<entity::MyEntity>)
        )
      ),
      Provider(
        Type(hana::type_c<Provider_>),
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
}//my_context

namespace test_stuff {

using OnlySupportedPath = typename decltype(
  nbdl::path_type<int, entity::Client, entity::MyEntity>)::type;
static_assert(std::is_same<typename OnlySupportedPath::Entity, entity::MyEntity>::value,
    "Only supported type entity is not MyEntity.");

struct TestProvider
{
  template<typename Fn>
  void read(OnlySupportedPath path, Fn fn)
  {
    if (path.getKey<entity::Client>() == 1 && path.getKey<entity::MyEntity>() == 5)
    {
      entity::MyEntity my_entity = { 5, 1 };
      fn(my_entity);
    }
    else
    {
      fn(nbdl::NotFound{});
    }
  }
};

class TestProviderAsync
{
  static std::function<void()> m_fn;

  public:

  static void flush__()
  {
    m_fn();
  }

  template<typename Fn>
  void read(OnlySupportedPath path, Fn fn)
  {
    m_fn = [path, fn]() {
      if (path.getKey<entity::Client>() == 1 && path.getKey<entity::MyEntity>() == 5)
      {
        entity::MyEntity my_entity = { 5, 1 };
        fn(my_entity);
      }
      else
      {
        fn(nbdl::NotFound{});
      }
    };
  }
};
std::function<void()> TestProviderAsync::m_fn = [](){};

}//test_stuff

TEST_CASE("Read an object from a context.", "[context]")
{
  bool result;
  auto ctx = my_context::create<test_stuff::TestProvider>();

  result = ctx->read(test_stuff::OnlySupportedPath(1, 5),
    [](nbdl::Unresolved) {
      return false;
    },
    [](nbdl::NotFound) {
      return false;
    },
    [](entity::MyEntity m) {
      return (m.id == 5 && m.client_id == 1);
    });

  CHECK(result);
}

TEST_CASE("Context should propagate NotFound from server callback.", "[context]")
{
  bool result = false;
  auto ctx = my_context::create<test_stuff::TestProvider>();

  result = ctx->read(test_stuff::OnlySupportedPath(1, 6),
    [](nbdl::Unresolved) {
      return false;
    },
    [](nbdl::NotFound) {
      return true;
    },
    [](entity::MyEntity const&) {
      return false;
    });

  CHECK(result);
}

TEST_CASE("Context should emit change to listener", "[context]")
{
  bool function_was_called = false;
  int result = 0;
  auto ctx = my_context::create<test_stuff::TestProviderAsync>();
  test_stuff::OnlySupportedPath path(1, 5);

  auto listener = ctx->makeListener(path, [&]() {
    function_was_called = true;
    result = ctx->read(path,
      [](nbdl::Unresolved) {
        return 1;
      },
      [](entity::MyEntity const&) {
        return 2;
      },
      [](nbdl::NotFound) {
        return 3;
      });
  });

  ctx->read(path, nbdl::noop);
  CHECK_FALSE(result); //does not emit for unresolved
  test_stuff::TestProviderAsync::flush__();
  CHECK(function_was_called);
  CHECK(result == 2); //entity::MyEntity
}

TEST_CASE("Context should not emit to a listener that has been destroyed.", "[context]")
{
  bool function_was_called = false;
  int result = 0;
  test_stuff::OnlySupportedPath path(1, 5);

  {
    auto ctx = my_context::create<test_stuff::TestProviderAsync>();
    /*
      in this test if the listener fails
      to unregister it would result in UB.
    */
    auto listener = ctx->makeListener(path, [&]() {
      function_was_called = true;
      result = ctx->read(path,
        [](nbdl::Unresolved) {
          return 1;
        },
        [](entity::MyEntity const&) {
          return 2;
        },
        [](nbdl::NotFound) {
          return 3;
        });
    });
    ctx->read(path, nbdl::noop);
  } //listener is destroyed and should unregister from the context

  CHECK(result == 0); //does not emit for unresolved
  test_stuff::TestProviderAsync::flush__();
  CHECK_FALSE(function_was_called);
  CHECK(result == 0);
}
