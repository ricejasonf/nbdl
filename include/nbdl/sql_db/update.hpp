//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_UPDATE_HPP
#define NBDL_SQL_DB_UPDATE_HPP

#include <nbdl/concept/Entity.hpp>
#include <nbdl/concept/EntityContainer.hpp>
#include <nbdl/fwd/sql_db/update.hpp>
#include <nbdl/sql_db/concept/TableEntity.hpp>
#include <nbdl/sql_db/traits.hpp>

#include <boost/hana/concat.hpp>
#include <boost/hana/core/default.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/find_if.hpp>
#include <boost/hana/optional.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <utility>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;

  template <typename DbUpdate, typename Entity, typename Keys>
  void update_fn::operator()(DbUpdate db_update, Entity&& e, Keys&& keys) const
  {
    using T = std::decay_t<Entity>;
    using Impl = update_impl<T>;

    constexpr auto is_primary_key = [](auto x)
      -> decltype(hana::and_(
        hana::first(x) == table_key_name<T>{})
      , hana::typeid_(hana::second(x)) == hana::type_c<primary_key<T>>
      )
    { };

    static_assert(
      decltype(hana::is_just(hana::find_if(keys, is_primary_key)))::value
    , "Update call must specify primary key for TableEntity."
    );

    Impl::apply(db_update, std::forward<Entity>(e), std::forward<Keys>(keys));
  };

  // one-to-one:
  //  child => parent w/child_id
  // one-to-many:
  //  parent => child w/parent_id

  template<typename Tag, bool condition>
  struct update_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static auto apply(...) = delete;
  };

  template <typename T>
  struct update_impl<T, hana::when<nbdl::Entity<T>::value>>
  {
    // Keys would be an owner_id or
    // the keys in a path from a nbdl::Message
    template <typename DbInsert, typename Entity, typename Keys>
    static void apply(DbInsert db_update, Entity&& e, Keys&& keys)
    {
      using ChildMembers = members_child<T>;
      using MultiMembers = members_multirow<T>;
      using ValueMembers = members_value<T>;

      static_assert(
        mp_size<ChildMembers>::value == 0
      , "Update for nested Entities not currently supported."
      );

      static_assert(
        mp_size<MultiMembers>::value == 0
      , "Update for nbdl::Entity with one-to-many relational members not supported."
      );

#if 0 // This should require getting the relevant keys from the database
      // so there is no chance that the client could spoof the keys.
      //
      // update children (one-to-one)
      // For TableEntity composition denotes row ownership despite
      // the table indirection. Using update with nested entities
      // does not provide any deduplication for "many-to-one"
      // relationships.
      hana::for_each(ChildMembers{}, [&](auto member)
      {
        // This requires that the key is present in
        // the C++ struct via member_key_name
        update(
          db_update
        , nbdl::get_member<decltype(member)>(std::forward<Entity>(e))
        , hana::make_map(
            hana::make_pair(
              member_key_name<decltype(member)>{}
            , nbdl::get(e, member_key_name<decltype(member)>{})
            )
          )
        );
      });
#endif

      // get "values" map
      auto values = hana::unpack(ValueMembers{}, [&](auto ...member)
      {
        return hana::make_tuple(
          hana::make_pair(
            nbdl::member_name<decltype(member)>{}
          , nbdl::get_member<decltype(member)>(std::forward<Entity>(e))
          )...
        );
      });

      // update self
      db_update(
        table_name<T>{}
      , std::move(values)
      , std::forward<Keys>(keys)
      );
    }
  };

  // TODO update_impl for nbdl::Delta

  template <typename T>
  struct update_impl<T, hana::when<nbdl::EntityContainer<T>::value>>
  {
    // I don't even know how this could be implemented
    static void apply(...) = delete;
  };
}

#endif
