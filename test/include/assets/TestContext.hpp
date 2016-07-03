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
  template<typename Provider1, typename Provider2, typename Consumer1, typename Consumer2, 
    typename Store_ = nbdl::null_store>
  constexpr auto make(
      Provider1 const& p1,
      Provider2 const& p2,
      Consumer1 const& c1,
      Consumer2 const& c2
  ) {
    return
      Context(
        Store(hana::type_c<Store_>),
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
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::root2>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::my_entity<2>>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::root2>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::my_entity<3>>),
                Actions(Create(), Read(), UpdateRaw(), Delete())
              )
            ),
            AccessPoint(
              Name(hana::type_c<void>),
              EntityName(hana::type_c<entity::root2>),
              AccessPoint(
                Name(hana::type_c<void>),
                EntityName(hana::type_c<entity::my_entity<4>>),
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
} // nbdl

#endif
