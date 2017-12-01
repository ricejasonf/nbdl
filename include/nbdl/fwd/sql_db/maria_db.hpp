//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_SQL_DB_MARIA_DB_HPP
#define NBDL_FWD_SQL_DB_MARIA_DB_HPP

#include<boost/hana/core/when.hpp>

namespace nbdl::sql_db::mariadb
{
  namespace hana = boost::hana;

  struct connection
  {
    template <typename Entity, typename Keys&&>
    auto select(Keys&&);

    template <typename Entity, typename Keys&&>
    void update(Entity&&, Keys&&);

    template <typename Entity>
    void insert(Entity&&);
  };

  template<typename T, typename = void>
  struct bind_column_to_impl : bind_column_to_impl<T, hana::when<true>> { };

  template<typename T, typename = void>
  struct bind_column_from_impl : bind_column_from_impl<T, hana::when<true>> { };
}

#endif
