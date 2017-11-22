//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_INSERT_HPP
#define NBDL_SQL_DB_INSERT_HPP

#include <nbdl/concept/Entity.hpp>
#include <nbdl/concept/EntityContainer.hpp>
#include <nbdl/fwd/sql_db/insert.hpp>
#include <nbdl/sql_db/concept/TableEntity.hpp>
#include <nbdl/sql_db/traits.hpp>

#include <boost/hana/concat.hpp>
#include <boost/hana/core/default.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/tuple.hpp>
#include <utility>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;

  template <typename DbInsert, typename Entity>
  auto insert_fn::operator()(DbInsert db_insert, Entity&& e) const
  {
    return this->operator()(db_insert, std::forward<Entity>(e), hana::tuple<>{});
  };

  template <typename DbInsert, typename Entity, typename ForeignKeys>
  auto insert_fn::operator()(DbInsert db_insert, Entity&& e, ForeignKeys&& keys) const
  {
    using Tag = std::decay_t<Entity>;
    using Impl = insert_impl<Tag>;

    return Impl::apply(db_insert, std::forward<Entity>(e), std::forward<ForeignKeys>(keys));
  };

  // one-to-one:
  //  child => parent w/child_id
  // one-to-many:
  //  parent => child w/parent_id

  template<typename Tag, bool condition>
  struct insert_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static auto apply(...) = delete;
  };

  template <typename T>
  struct insert_impl<T, hana::when<nbdl::Entity<T>::value>>
  {
    // ForeignKeys would be an owner_id or
    // the keys in a path from a nbdl::Message
    template <typename DbInsert, typename Entity, typename ForeignKeys>
    static auto apply(DbInsert db_insert, Entity&& e, ForeignKeys&& keys)
      -> primary_key<T>
    {
      using ChildMembers = members_child<T>;
      using MultiMembers = members_multirow<T>;
      using ValueMembers = members_value<T>;

      // insert children (one-to-one)
      auto child_ids = hana::unpack(ChildMembers{}, [&](auto ...member)
      {
        return hana::make_tuple(
          hana::make_pair(
            member_key_name<decltype(member)>{}
          , insert(db_insert, nbdl::get_member<decltype(member)>(std::forward<Entity>(e)))
          )...
        );
      });

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

      // insert self
      primary_key<T> insert_id{db_insert(
        table_name<T>{}
      , hana::concat(
          hana::concat(
            std::forward<ForeignKeys>(keys)
          , std::move(child_ids)
          )
        , std::move(values)
        )
      )};

      // insert children in containers (one-to-many)
      hana::for_each(MultiMembers{}, [&](auto member)
      {
        insert(
          db_insert
        , nbdl::get_member<decltype(member)>(std::forward<Entity>(e))
        , hana::make_tuple(
            hana::make_pair(
              table_key_name<T>{}
            , insert_id
            )
          )
        );
      });

      return insert_id;
    }
  };

  template <typename T>
  struct insert_impl<T, hana::when<nbdl::EntityContainer<T>::value>>
  {
    template <typename DbInsert, typename Container, typename ForeignKeys>
    static void apply(DbInsert db_insert, Container&& c, ForeignKeys&& keys)
    {
      for (auto&& x : std::forward<Container>(c))
      {
        insert(
          db_insert
        , std::forward<decltype(x)>(x)
        , std::forward<ForeignKeys>(keys)
        );
      }
    };
  };
}

#endif
