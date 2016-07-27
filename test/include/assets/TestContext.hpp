//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NBDL_TEST_ASSETS_TEST_CONTEXT_HPP
#define NBDL_TEST_ASSETS_TEST_CONTEXT_HPP

#include <def/builder/Context.hpp>
#include <def/directives.hpp>
#include <nbdl/entity.hpp>
#include <nbdl/null_store.hpp>
#include <Path.hpp>

#include <boost/hana.hpp>
#include <utility>
#include <vector>

namespace hana = boost::hana;

namespace test_context {
  namespace name {
    template<int i> struct provider_t { };
    template<int i>
    constexpr auto provider = boost::hana::type_c<provider_t<i>>;

    template<int i> struct consumer_t { };
    template<int i>
    constexpr auto consumer = boost::hana::type_c<consumer_t<i>>;
  } // name

  namespace entity {
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
} // test_context

namespace nbdl {
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
        Entities(
          Entity(
            Type(hana::type_c<entity::root1>)
          ),
          Entity(
            Type(hana::type_c<entity::root2>)
          ),
          Entity(Type(hana::type_c<entity::my_entity<1>>)),
          Entity(Type(hana::type_c<entity::my_entity<2>>)),
          Entity(Type(hana::type_c<entity::my_entity<3>>)),
          Entity(Type(hana::type_c<entity::my_entity<4>>))
        ),
        Providers(
          Provider(
            Name(test_context::name::provider<1>),
            Type(p1),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::root1>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::my_entity<1>>),
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
              EntityName(hana::type_c<entity::root2>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::my_entity<1>>),
                Store(hana::type_c<Store_>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::root2>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::my_entity<2>>),
                Store(hana::type_c<Store_>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::root2>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::my_entity<3>>),
                Store(hana::type_c<Store_>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::root2>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::my_entity<4>>),
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
    using MessageVariant = typename decltype(std::declval<PushApi>()
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

    using MessageVariant = typename decltype(std::declval<PushApi>()
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

    using MessageVariant = typename decltype(std::declval<PushApi>()
      .template get_downstream_variant_type<null_system_message>())::type;

    PushApi push_api;
    std::vector<MessageVariant> recorded_messages;

    template<typename P>
    consumer(P&& p)
      : push_api(std::forward<P>(p))
      , recorded_messages()
    { }
  };

  using path1 = typename decltype(
    nbdl::path_type<int, entity::root1, entity::my_entity<1>>
  )::type;

  template <int i>
  using path = typename decltype(
    nbdl::path_type<
      int,
      decltype(hana::if_(hana::bool_c<i == 0>, entity::root1{}, entity::root2{})),
      decltype(hana::if_(hana::bool_c<i == 0>, entity::my_entity<1>{}, entity::my_entity<i>{}))
    >
  )::type;
    
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

  struct mock_store_tag { };

  template <typename Path>
  struct mock_store
  {
    using hana_tag = mock_store_tag;
    using path = Path;
    using entity = typename Path::entity;
  };

} // test_context

namespace
{
  using test_context::path_variant;
  path_variant mock_store_result_apply_action{};
  hana::tuple<
    path_variant,             // path of store that is listening
    path_variant,             // path of message to listen for
    std::vector<path_variant> // paths that will be marked as changed as a result of the message
  >  mock_store_result_apply_foreign_action{};
}

namespace nbdl
{
  // Provider

  template <>
  struct make_provider_impl<test_context::provider_tag>
  {
    template <typename ...Args>
    static constexpr auto apply(Args&& ...args)
    {
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

  // Store - mock_store

  template <>
  struct make_store_impl<test_context::mock_store_tag>
  {
    template <typename PathType>
    static constexpr auto apply(PathType)
      -> test_context::mock_store<typename PathType::type>
    { return {}; }
  };

  template <>
  struct get_impl<test_context::mock_store_tag>
  {
    template <typename Store, typename Path>
    static constexpr auto apply(Store&&, Path const&)
      -> typename test_context::mock_store<Path>::entity
    { return {}; }
  };

  template <>
  struct apply_action_impl<test_context::mock_store_tag>
  {
    template <typename Store, typename Message>
    static constexpr auto apply(Store&&, Message const& m)
    {
      using Path = std::decay_t<decltype(message::get_path(m))>;
      return mock_store_result_apply_action.match(
        [&](Path const& p)
        {
          return hana::equal(p, message::get_path(m));
        },
        [](auto const&) { return false; }
      );
    }
  };

  template <>
  struct apply_foreign_action_impl<test_context::mock_store_tag>
  {
    template <typename Store, typename Message, typename Fn>
    static constexpr auto apply(Store const&, Message const& m, Fn const& fn)
    {
      using Path = typename std::decay_t<Store>::path;
      using MessagePath = typename std::decay_t<decltype(message::get_path(m))>;
      hana::at_c<0>(mock_store_result_apply_foreign_action).match([&](Path const&)
      {
        hana::at_c<1>(mock_store_result_apply_foreign_action).match([&](MessagePath const& mp)
        {
          if (hana::equal(mp, message::get_path(m)))
          {
            for (auto const& v : hana::at_c<2>(mock_store_result_apply_foreign_action))
            {
              v.match([&](Path const& p_)
              {
                fn(p_);
              }, nbdl::noop);
            }
          }
        }, nbdl::noop);
      }, nbdl::noop);
    }
  };

} // nbdl

#endif
