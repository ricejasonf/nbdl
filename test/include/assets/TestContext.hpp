//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NBDL_TEST_ASSETS_TEST_CONTEXT_HPP
#define NBDL_TEST_ASSETS_TEST_CONTEXT_HPP

#include<def/builder/Context.hpp>
#include<def/directives.hpp>
#include<macros/NBDL_ENTITY.hpp>
#include<Path.hpp>

#include<boost/hana.hpp>

namespace test_context {
  namespace name {
    template<int i> struct Provider { };
    template<int i>
    constexpr auto provider = boost::hana::type_c<Provider<i>>;

    template<int i> struct Consumer { };
    template<int i>
    constexpr auto consumer = boost::hana::type_c<Consumer<i>>;
  } // name

  namespace entity {
    struct Root1
    {
      int id;
    };
    struct Root2
    {
      int id;
    };
    struct MyEntity
    {
      int id;
      int root_id;
    };
  } // entity
} // test_context

namespace nbdl {
  NBDL_ENTITY(
    test_context::entity::Root1,
      id);
  NBDL_ENTITY(
    test_context::entity::Root2,
      id);
  NBDL_ENTITY(
    test_context::entity::MyEntity,
      id,
      root_id );
} // nbdl

namespace test_context_def {
  namespace hana = boost::hana;
  namespace entity = test_context::entity;
  using namespace nbdl_def;

  template<typename Provider1, typename Provider2, typename Consumer1, typename Consumer2>
  constexpr auto make(
      Provider1 const& p1,
      Provider2 const& p2,
      Consumer1 const& c1,
      Consumer2 const& c2
  ) {
    return
      Context(
        Entities(
          Entity(
            Type(hana::type_c<entity::Root1>)
          ),
          Entity(
            Type(hana::type_c<entity::Root2>)
          ),
          Entity(
            Type(hana::type_c<entity::MyEntity>)
          )
        ),
        Providers(
          Provider(
            Name(test_context::name::provider<1>),
            Type(p1),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::Root1>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::MyEntity>),
                Actions(Read())
              )
            )
          ),
          Provider(
            Name(test_context::name::provider<2>),
            Type(p2),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::Root2>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::MyEntity>),
                Actions(Read())
              )
            )
          )
        ),
        Consumers(
          Consumer(
            Name(test_context::name::consumer<1>),
            Type(c1)
            ),
          Consumer(
            Name(test_context::name::consumer<2>),
            Type(c2)
          )
        )
      );
  }

} // test_context_def

namespace test_context {
  template<int i>
  struct IntTag
  {
    int x;
    IntTag() : x(i) { }
  };

  template<typename PushFn, typename T = void>
  struct Provider
  {
    PushFn push;
    T t_;

    template<typename P, typename A>
    Provider(P&& p, A&& a)
      : push(std::forward<P>(p))
      , t_(std::forward<A>(a))
    { }
  };

  template<typename PushFn>
  struct Provider<PushFn, void>
  {
    PushFn push;

    template<typename P>
    Provider(P&& p)
      : push(std::forward<P>(p))
    { }
  };

  template<typename PushFn, typename T = void>
  struct Consumer
  {
    PushFn push;
    T t_;

    template<typename P, typename A>
    Consumer(P&& p, A&& a)
      : push(std::forward<P>(p))
      , t_(std::forward<A>(a))
    { }
  };

  template<typename PushFn>
  struct Consumer<PushFn, void>
  {
    PushFn push;

    template<typename P>
    Consumer(P&& p)
      : push(std::forward<P>(p))
    { }
  };

  using Path1 = typename decltype(
    nbdl::path_type<int, entity::Root1, entity::MyEntity>
  )::type;

  using Path2 = typename decltype(
    nbdl::path_type<int, entity::Root2, entity::MyEntity>
  )::type;
} // test_context

#endif
