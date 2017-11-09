//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_ACTION_HPP
#define NBDL_SQL_DB_ACTION_HPP


#include <sqlpp11/something.h>
#include <utility>

namespace nbdl::sql_db
{
  template <typename Resolver, typename Db, typename Path, typename Entity>
  auto create_fn::operator()(Resolver& r, Db& db, Path&& p, Entity&& e)
  {
    // TODO
    constexpr auto table = table_of<Entity>;
    db.run(sqlpp::insert_into(table, to_members(std::forward<Entity>(e))));
  }

  template <typename Resolver, typename Db, typename Path>
  auto read_fn::operator()(Resolver& r, Db& db, Path&& p)
  {
    // TODO
  }

  template <typename Resolver, typename Db, typename Path, typename Entity>
  auto update_fn::operator()(Resolver& r, Db& db, Path&& p, Entity&& e)
  {
    // TODO
  }

  template <typename Resolver, typename Db, typename Path>
  auto delete_fn::operator()(Resolver& r, Db& db, Path&& p)
  {
    // TODO
  }
}

#endif
