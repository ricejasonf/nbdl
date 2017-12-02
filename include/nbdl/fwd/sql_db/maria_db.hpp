//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_SQL_DB_MARIA_DB_HPP
#define NBDL_FWD_SQL_DB_MARIA_DB_HPP

#include <boost/hana/core/when.hpp>
#include <mysql.h>

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

  struct input_column_info
  {
    MYSQL_BIND& bind;
    my_bool is_null;
  };

  struct output_column_info
  {
    MYSQL_BIND& bind;
    unsigned long length;
    int column_index;
    my_bool is_null;
    my_bool is_truncated;
  };

  template<typename Tag, typename = void>
  struct bind_column_input_impl : bind_column_input_impl<Tag, hana::when<true>> { };

  template<typename Tag, typename = void>
  struct bind_column_output_impl : bind_column_output_impl<Tag, hana::when<true>> { };

  template<typename Tag, typename = void>
  constexpr auto column_type_name_impl = column_type_impl<Tag, hana::when<true>>{};
}

#endif
