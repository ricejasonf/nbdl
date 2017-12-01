//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_MARIA_DB_HPP
#define NBDL_SQL_DB_MARIA_DB_HPP

#include <nbdl/sql_db/insert.hpp>
#include <nbdl/sql_db/select.hpp>
#include <nbdl/sql_db/statement/insert.hpp>
#include <nbdl/sql_db/statement/select.hpp>
#include <nbdl/sql_db/statement/update.hpp>
#include <nbdl/sql_db/update.hpp>
#include <nbdl/concept/String.hpp>
#include <nbdl/concept/Buffer.hpp>

#include <boost/hana/if.hpp>
#include <mysql.h>
#include <type_traits>
#include <utility>

namespace nbdl::sql_db::mariadb
{
  namespace hana = boost::hana;

  template <typename Entity, typename Keys>
  auto connection::select(Keys&& keys)
  {
    // TODO
  };

  template <typename Keys>
  void connection::insert(Keys&& keys)
  {
    // TODO
  };

  template <typename Entity, typename Keys>
  void connection::update(Entity&& entity, Keys&& keys)
  {
    // TODO
  };

  namespace detail
  {
    constexpr auto bind_column_to = [](...)
    {

    };

    constexpr auto bind_column_from = [](...)
    {

    };
  }

  template <typename Tag, bool condition>
  struct bind_column_to_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    template <typename T>
    static void apply(T&, MYSQL_BIND&) = delete;
  };

  template <typename Tag, bool condition>
  struct bind_column_from_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    template <typename T>
    static void apply(T& entity, MYSQL_BIND& mysql_bind)
    {
      bind_to_impl<Tag>::apply(
        entity
      , mysql_bind
      );
    }
  };

  //
  // Bind Impls
  //

  // DynamicBuffer (includes strings)

  template <typename Tag>
  struct bind_column_to_impl<Tag, hana::when<nbdl::DynamicBuffer<Tag>::value>>
  {
    template <typename T>
    static void apply(T& e, MYSQL_BIND& b)
    {
      b.buffer_type   = hana::if_(nbdl::String<Tag>{}, MYSQL_TYPE_VAR_STRING , MYSQL_BLOB);
      b.buffer        = e.data();
      b.buffer_length = e.size();
    }
  };

  template <typename Tag>
  struct bind_column_from_impl<Tag, hana::when<nbdl::DynamicBuffer<Tag>::value>>
  {
    template <typename T>
    static void apply(T& e, MYSQL_BIND& b)
    {
      b.buffer_type = hana::if_(nbdl::String<Tag>{}, MYSQL_TYPE_VAR_STRING , MYSQL_BLOB);
      b.buffer = e.data();
      b.buffer_length = e.size();
    }
  };

  // Buffer

  template <typename Tag>
  struct bind_column_to_impl<Tag, hana::when<(nbdl::Buffer<Tag>::value)>>
  {
    // TODO
  };

  // Integral

  template <typename Tag>
  struct bind_column_to_impl<Tag, hana::when<std::is_integral<Tag>::value>>
  {
    // TODO
  };

  // FloatingPoint (float, double, long double, double double w/cheese)

  template <typename Tag>
  struct bind_column_to_impl<Tag, hana::when<std::is_floating_point<Tag>::value>>
  {
    // TODO
  };
}

#endif
