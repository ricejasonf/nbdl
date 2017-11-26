//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_SELECT_HPP
#define NBDL_SQL_DB_SELECT_HPP

#include <mpdef/list.hpp>
#include <nbdl/fwd/sql_db/select.hpp>
#include <nbdl/sql_db/statement/detail/table_alias_list.hpp>

#include <boost/hana/core/default.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/mp11/list.hpp>
#include <utility>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  template <typename Entity>
  template <typename DbSelect, typename Keys>
  auto select_fn<Entity>::operator()(DbSelect db_select, Keys&& keys) const
  {
    using Impl = select_impl<Entity>;

    return Impl::apply(db_select, std::forward<Keys>(keys));
  };

  template<typename T, bool condition>
  struct select_impl<T, hana::when<condition>>
    : hana::default_
  {
    static void apply(...) = delete;
  };

  template <typename T>
  struct select_impl<T, hana::when<nbdl::Entity<T>::value>>
  {
    // Keys would be an owner_id or
    // the keys in a path from a nbdl::Message
    template <typename DbSelect, typename Keys>
    static auto apply(DbSelect db_select, Keys&& keys)
    {
      static_assert(
        mp_size<members_multirow<T>>::value == 0
      , "Select for TableEntity with one-to-many relational members not currently supported."
      );

      return db_select(
        table_name<T>{}
      , statement::detail::columns_clause<T>{}
      , statement::detail::join_clauses<T>{}
      , std::forward<Keys>(keys)
      );
    }
  };

  template <typename T>
  struct select_impl<T, hana::when<nbdl::EntityContainer<T>::value>>
  {
    static void apply(...) = delete;
  };
}

#endif
