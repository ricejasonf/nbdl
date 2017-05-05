//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NBDL_TEST_ASSETS_TEST_CONTEXT_HPP
#define NBDL_TEST_ASSETS_TEST_CONTEXT_HPP

#include <mpdef/list.hpp>
#include <nbdl/context.hpp>
#include <nbdl/def/builder/context.hpp>
#include <nbdl/def/directives.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/null_store.hpp>

#include <boost/hana.hpp>
#include <utility>
#include <vector>

namespace hana = boost::hana;

namespace test_context
{
  namespace name
  {
    template<int i> struct provider_t { };
    template<int i>
    constexpr auto provider = boost::hana::type_c<provider_t<i>>;

    template<int i> struct consumer_t { };
    template<int i>
    constexpr auto consumer = boost::hana::type_c<consumer_t<i>>;
  } // name

  namespace entity
  {
    struct root1
    {
      int id;
    };
    struct root2
    {
      int id;
    };
    template <int>
    struct my_entity
    {
      int id;
      int root_id;
    };
  } // entity

  template <typename Entity>
  struct key
  {
    int value;

    key(int v)
      : value(v)
    { }
  };
} // test_context

namespace boost::hana
{
  template <typename Entity>
  struct equal_impl<test_context::key<Entity>, test_context::key<Entity>>
  {
    template <typename T1, typename T2>
    static constexpr auto apply(T1 const& t1, T2 const& t2)
    { return t1.value == t2.value; }
  };
}

namespace nbdl
{
  NBDL_ENTITY(
    test_context::entity::root1,
      id);
  NBDL_ENTITY(
    test_context::entity::root2,
      id);
  NBDL_ENTITY(
    test_context::entity::my_entity<1>,
      id,
      root_id );
  NBDL_ENTITY(
    test_context::entity::my_entity<2>,
      id,
      root_id );
  NBDL_ENTITY(
    test_context::entity::my_entity<3>,
      id,
      root_id );
  NBDL_ENTITY(
    test_context::entity::my_entity<4>,
      id,
      root_id );

  template <typename Entity>
  struct entity_members_impl<test_context::key<Entity>>
  { 
    using type = mpdef::list<NBDL_MEMBER(&test_context::key<Entity>::value)>;
  };
} // nbdl

namespace test_context_def {
  namespace hana = boost::hana;
  namespace entity = test_context::entity;
  using namespace nbdl_def;

  // Provider and Consumer are actually tags
  template<
    typename Provider1,
    typename Provider2,
    typename Consumer1,
    typename Consumer2,
    typename Store_
  >
  constexpr auto make(
      Provider1 const& p1,
      Provider2 const& p2,
      Consumer1 const& c1,
      Consumer2 const& c2,
      Store_ const&
  ) {
    return
      Context(
        Providers(
          Provider(
            Name(test_context::name::provider<1>),
            Type(p1),
            AccessPoint(
              Name(hana::type_c<void>),
              Entity<entity::root1>,
              PathKey<test_context::key<entity::root1>>,
              AccessPoint(
                Name(hana::type_c<void>),
                Entity<entity::my_entity<1>>,
                PathKey<test_context::key<entity::my_entity<1>>>,
                Store(hana::type_c<Store_>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            )
          ),
          Provider(
            Name(test_context::name::provider<2>),
            Type(p2),
            AccessPoint(
              Name(hana::type_c<void>),
              Entity<entity::root2>,
              PathKey<test_context::key<entity::root2>>,
              AccessPoint(
                Name(hana::type_c<void>),
                Entity<entity::my_entity<1>>,
                PathKey<test_context::key<entity::my_entity<1>>>,
                Store(hana::type_c<Store_>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              Entity<entity::root2>,
              PathKey<test_context::key<entity::root2>>,
              AccessPoint(
                Name(hana::type_c<void>),
                Entity<entity::my_entity<2>>,
                PathKey<test_context::key<entity::my_entity<2>>>,
                Store(hana::type_c<Store_>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              Entity<entity::root2>,
              PathKey<test_context::key<entity::root2>>,
              AccessPoint(
                Name(hana::type_c<void>),
                Entity<entity::my_entity<3>>,
                PathKey<test_context::key<entity::my_entity<3>>>,
                Store(hana::type_c<Store_>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              Entity<entity::root2>,
              PathKey<test_context::key<entity::root2>>,
              AccessPoint(
                Name(hana::type_c<void>),
                Entity<entity::my_entity<4>>,
                PathKey<test_context::key<entity::my_entity<4>>>,
                Store(hana::type_c<Store_>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
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
  struct int_tag
  {
    int x;
    int_tag() : x(i) { }
  };

  struct provider_tag { };
  struct consumer_tag { };
  struct state_consumer_tag { };
  struct null_system_message { };

  template<typename PushApi, typename T = void>
  struct provider
  {
    using hana_tag = test_context::provider_tag;
    PushApi push_api;
    T t_;

    template<typename P, typename A>
    provider(P&& p, A&& a)
      : push_api(std::forward<P>(p))
      , t_(std::forward<A>(a))
    { }
  };

  template<typename PushApi>
  struct provider<PushApi, void>
  {
    using hana_tag = test_context::provider_tag;

    // If there are other providers the variants supplied by the PushApi
    // will probably not be sufficient.
    using MessageVariant = typename decltype(std::declval<PushApi>().message_api()
      .template get_upstream_variant_type<null_system_message>())::type;

    PushApi push_api;
    std::vector<MessageVariant> recorded_messages;

    template<typename P>
    provider(P&& p)
      : push_api(std::forward<P>(p))
      , recorded_messages()
    { }
  };

  template<typename PushApi, typename T = void>
  struct consumer
  {
    using hana_tag = test_context::consumer_tag;

    using MessageVariant = typename decltype(std::declval<PushApi>().message_api()
      .template get_downstream_variant_type<null_system_message>())::type;

    PushApi push_api;
    T t_;
    std::vector<MessageVariant> recorded_messages;

    template<typename P, typename A>
    consumer(P&& p, A&& a)
      : push_api(std::forward<P>(p))
      , t_(std::forward<A>(a))
      , recorded_messages()
    { }
  };

  template<typename PushApi>
  struct consumer<PushApi, void>
  {
    using hana_tag = test_context::consumer_tag;

    using MessageVariant = typename decltype(std::declval<PushApi>().message_api()
      .template get_downstream_variant_type<null_system_message>())::type;

    PushApi push_api;
    std::vector<MessageVariant> recorded_messages;

    template<typename P>
    consumer(P&& p)
      : push_api(std::forward<P>(p))
      , recorded_messages()
    { }
  };

  using path1 = hana::tuple<key<entity::root1>, key<entity::my_entity<1>>>;

  template <int i>
  using path = hana::tuple<
    key<decltype(hana::if_(hana::bool_c<i == 0>, entity::root1{}, entity::root2{}))>,
    key<decltype(hana::if_(hana::bool_c<i == 0>, entity::my_entity<1>{}, entity::my_entity<i>{}))>
  >;

  using path_variant = nbdl::variant<path<0>, path<1>, path<2>, path<3>, path<4>>;

  template <typename PushApi>
  struct state_consumer
  {
    using hana_tag = test_context::state_consumer_tag;

    PushApi push_api;
    std::vector<path_variant> recorded_notifications;

    template <typename P>
    state_consumer(P&& p)
      : push_api(std::forward<P>(p))
      , recorded_notifications()
    { }
  };
} // test_context

namespace nbdl
{
  // Provider

  template <>
  struct make_provider_impl<test_context::provider_tag>
  {
    template <typename ...Args>
    static constexpr auto apply(Args&& ...args)
    {
      static_assert(
        !decltype(hana::contains(hana::make_tuple(hana::typeid_(args)...), hana::type_c<void>)){}
      , "Type void should not be passed as parameter to context element"
      );
      return test_context::provider<std::decay_t<Args>...>(std::forward<Args>(args)...);
    }
  };

  template <>
  struct send_upstream_message_impl<test_context::provider_tag>
  {
    template <typename Provider, typename Message>
    static constexpr auto apply(Provider& p, Message&& m)
    {
      p.recorded_messages.push_back(std::forward<Message>(m));
    }
  };

  // Consumer

  template <>
  struct make_consumer_impl<test_context::consumer_tag>
  {
    template <typename ...Args>
    static constexpr auto apply(Args&& ...args)
    {
      static_assert(
        !decltype(hana::contains(hana::make_tuple(hana::typeid_(args)...), hana::type_c<void>)){}
      , "Type void should not be passed as parameter to context element"
      );
      return test_context::consumer<std::decay_t<Args>...>(std::forward<Args>(args)...);
    }
  };

  template <>
  struct send_downstream_message_impl<test_context::consumer_tag>
  {
    template <typename Provider, typename Message>
    static constexpr auto apply(Provider& p, Message&& m)
    {
      p.recorded_messages.push_back(std::forward<Message>(m));
    }
  };

  // StateConsumer

  template <>
  struct make_state_consumer_impl<test_context::state_consumer_tag>
  {
    template <typename PushApi>
    static constexpr auto apply(PushApi&& p)
    {
      return test_context::state_consumer<std::decay_t<PushApi>>(std::forward<PushApi>(p));
    }
  };

  template <>
  struct notify_state_change_impl<test_context::state_consumer_tag>
  {
    template <typename StateConsumer, typename Path>
    static constexpr void apply(StateConsumer&& s, Path&& p)
    {
      std::forward<StateConsumer>(s).recorded_notifications.push_back(std::forward<Path>(p));
    }
  };
} // nbdl

#endif
