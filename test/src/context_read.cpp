//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<functional>
#include<boost/hana.hpp>
#include<nbdl>
#include "catch.hpp"

namespace hana = boost::hana;

namespace test_stuff {

struct Client
{
  int id;
};
struct MyEntity
{
  int id;
  int client_id;
};
}//test_stuff
//todo don't use template specialization
//because it is global and
//it causes problems like this
//(specializing inside nbdl namespace)
namespace nbdl {
  NBDL_ENTITY(
    test_stuff::Client,
      id);
  NBDL_ENTITY(
    test_stuff::MyEntity,
      id,
      client_id );
}//nbdl

namespace test_stuff {

using OnlySupportedPath = typename nbdl::CreatePath<Client, MyEntity>::Type;
static_assert(std::is_same<typename OnlySupportedPath::Entity, MyEntity>::value,
    "Only supported type entity is not MyEntity.");

struct TestClient
{
  template<typename Fn>
  void read(OnlySupportedPath path, Fn fn)
  {
    if (path.getKey<Client>() == 1 && path.getKey<MyEntity>() == 5)
    {
      MyEntity my_entity = { 5, 1 };
      fn(my_entity);
    }
    else
    {
      fn(nbdl::NotFound{});
    }
  }
};

class TestClientAsync
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
      if (path.getKey<Client>() == 1 && path.getKey<MyEntity>() == 5)
      {
        MyEntity my_entity = { 5, 1 };
        fn(my_entity);
      }
      else
      {
        fn(nbdl::NotFound{});
      }
    };
  }
};
std::function<void()> TestClientAsync::m_fn = [](){};

}//test_stuff

namespace my_nbdl {
  using namespace nbdl_ddl;

  auto createContext = nbdl_ddl::buildContextFactory(
    nbdl_ddl::Context(
      Client(boost::hana::type_c<test_stuff::TestClient>),
      Api(
        AccessPoint(
          Path(boost::hana::type_c<test_stuff::OnlySupportedPath>),
          Actions(BOOST_HANA_STRING("Read"))
        )
      )
    )
  );
}//my_nbdl

TEST_CASE("Read an object from a context.", "[context]") 
{
  bool result;
  //auto ctx = MyContext::create(TestClient{});
  auto ctx = my_nbdl::createContext(test_stuff::TestClient{});

  result = ctx->read(test_stuff::OnlySupportedPath(1, 5),
    [](nbdl::Unresolved) {
      return false;
    },
    [](nbdl::NotFound) {
      return false;
    },
    [](test_stuff::MyEntity m) {
      return (m.id == 5 && m.client_id == 1);
    });

  CHECK(result);
}

TEST_CASE("Context should propagate NotFound from server callback.", "[context]") 
{
  bool result = false;
  //auto ctx = MyContext::create(TestClient{});
  auto ctx = my_nbdl::createContext(test_stuff::TestClient{});

  result = ctx->read(test_stuff::OnlySupportedPath(1, 6),
    [](nbdl::Unresolved) {
      return false;
    },
    [](nbdl::NotFound) {
      return true;
    },
    [](test_stuff::MyEntity) {
      return false;
    });

  CHECK(result);
}

/*
using MyContextAsync = nbdl::Context<
  TestClientAsync,
  nbdl::ListenerHandlerDummy<>,
  nbdl_def::Api<
    nbdl_def::AccessPoint<
      OnlySupportedPath,
      nbdl_def::Actions<nbdl_def::Read>
    >
  >>;
*/
namespace my_nbdl_async {
  using namespace nbdl_ddl;
  auto createContext = buildContextFactory(
    Context(
      Client(boost::hana::type_c<test_stuff::TestClientAsync>),
      Api(
        AccessPoint(
          Path(boost::hana::type_c<test_stuff::OnlySupportedPath>),
          Actions(BOOST_HANA_STRING("Read"))
        )
      )
    )
  );
}//my_nbdl_async

TEST_CASE("Context should emit change to listener", "[context]")
{
  bool function_was_called = false;
  int result = 0;
  //auto ctx = MyContextAsync::create(TestClientAsync{});
  auto ctx = my_nbdl_async::createContext(test_stuff::TestClientAsync{});
  test_stuff::OnlySupportedPath path(1, 5);

  auto listener = ctx->makeListener(path, [&]() {
    function_was_called = true;
    result = ctx->read(path,
      [](nbdl::Unresolved) {
        return 1;
      },
      [](test_stuff::MyEntity) {
        return 2;
      },
      [](nbdl::NotFound) {
        return 3;
      });
  });

  ctx->read(path, nbdl::noop);
  CHECK_FALSE(result); //does not emit for unresolved
  test_stuff::TestClientAsync::flush__();
  CHECK(function_was_called);
  CHECK(result == 2); //MyEntity
}

TEST_CASE("Context should not emit to a listener that has been destroyed.", "[context]")
{
  bool function_was_called = false;
  int result = 0;
  test_stuff::OnlySupportedPath path(1, 5);

  {
    //auto ctx = MyContext::create(TestClient{});
    auto ctx = my_nbdl_async::createContext(test_stuff::TestClientAsync{});
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
        [](test_stuff::MyEntity) {
          return 2;
        },
        [](nbdl::NotFound) {
          return 3;
        });
    });
    ctx->read(path, nbdl::noop);
  } //listener is destroyed and should unregister from the context

  CHECK(result == 0); //does not emit for unresolved
  test_stuff::TestClientAsync::flush__();
  CHECK_FALSE(function_was_called);
  CHECK(result == 0);
}
