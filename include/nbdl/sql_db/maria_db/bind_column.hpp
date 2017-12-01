//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_MARIA_DB_BIND_COLUMN_HPP
#define NBDL_SQL_DB_MARIA_DB_BIND_COLUMN_HPP

#include <nbdl/concept/Buffer.hpp>
#include <nbdl/concept/DynamicBuffer.hpp>
#include <nbdl/concept/String.hpp>

#include <boost/hana/if.hpp>
#include <mysql.h>
#include <type_traits>
#include <utility>

namespace nbdl::sql_db::mariadb
{
  namespace hana = boost::hana;

  namespace detail
  {
    constexpr auto bind_column_input = [](auto& entity, auto& column_info)
    {
      using Tag = hana::tag_of_t<decltype(entity)>; 
      using Impl = bind_column_input_impl<Tag>;

      Impl::apply(entity, column_info);
    };

    constexpr auto bind_column_output = [](auto& entity, auto& column_info)
    {
      using Tag = hana::tag_of_t<decltype(entity)>; 
      using Impl = bind_column_output_impl<Tag>;

      Impl::apply(entity, column_info);
    };

    template <std::size_t s> std::size_t integral_type = -1;
    template <> enum_field_types integral_type<1> = MYSQL_TYPE_TINY;
    template <> enum_field_types integral_type<2> = MYSQL_TYPE_SHORT;
    template <> enum_field_types integral_type<4> = MYSQL_TYPE_LONG;
    template <> enum_field_types integral_type<8> = MYSQL_TYPE_LONGLONG;
  }

  template <typename Tag, bool condition>
  struct bind_column_input_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    template <typename T>
    static void apply(T&, MYSQL_BIND&) = delete;
  };

  template <typename Tag, bool condition>
  struct bind_column_output_impl<Tag, hana::when<condition>>
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
  struct bind_column_input_impl<Tag, hana::when<nbdl::DynamicBuffer<Tag>::value>>
  {
    template <typename T>
    static void apply(T& e, input_column_info& info)
    {
      info.bind.buffer_type   = hana::if_(nbdl::String<Tag>{}, MYSQL_TYPE_VAR_STRING, MYSQL_TYPE_BLOB);
      info.bind.buffer        = e.data();
      info.bind.buffer_length = e.size();
    }
  };

  template <typename Tag>
  struct bind_column_output_impl<Tag, hana::when<nbdl::DynamicBuffer<Tag>::value>>
  {
    template <typename T>
    static void apply(T& e, output_column_info& info)
    {
      if (not info.is_truncated)
      {
        info.bind.buffer_type = hana::if_(nbdl::String<Tag>{}, MYSQL_TYPE_VAR_STRING, MYSQL_TYPE_BLOB);
        info.bind.buffer      = e.data();
        info.bind.length      = &info.length;
        info.bind.error       = info.is_truncated;
      }
      else
      {
        // rebind once we know the length of the field
        e.resize(info.length, '\0');
        mysql_stmt_fetch_column(stmt, bind, info.column_index, 0);
      }
    }
  };

  // Buffer

  template <typename Tag>
  struct bind_column_input_impl<Tag, hana::when<(nbdl::Buffer<Tag>::value)>>
  {
    template <typename T>
    static void apply(T& e, input_column_info& info)
    {
      info.bind.buffer_type   = hana::if_(nbdl::String<Tag>{}, MYSQL_TYPE_VAR_STRING, MYSQL_TYPE_BLOB);
      info.bind.buffer        = e.data();
      info.bind.buffer_length = e.size();
    }
  };

  template <typename Tag>
  struct bind_column_output_impl<Tag, hana::when<nbdl::Buffer<Tag>::value>>
  {
    template <typename T>
    static void apply(T& e, output_column_info& info)
    {
      if (not info.is_truncated)
      {
        info.bind.buffer_type = hana::if_(nbdl::String<Tag>{}, MYSQL_TYPE_VAR_STRING, MYSQL_TYPE_BLOB);
        info.bind.buffer      = e.data();
        info.bind.length      = &info.length;
        info.bind.error       = info.is_truncated;
      }
      else
      {
        mysql_stmt_fetch_column(stmt, bind, info.column_index, 0);
      }
    }
  };

  // Integral

  template <typename Tag>
  struct bind_column_input_impl<Tag, hana::when<std::is_integral<Tag>::value>>
  {
    template <typename T, typename ColumnInfo>
    static void apply(T& e, ColumnInfo& info)
    {
      using Integer = std::decay_t<T>;
      info.bind.buffer_type   = detail::integral_type<sizeof(Integer)>;
      info.bind.buffer        = e.data();
      info.bind.buffer_length = e.size();
      info.bind.is_unsigned   = std::is_unsigned<Integer>::value;
    }
  };

  // FloatingPoint (float, double, long double, double double w/cheese)

  template <typename Tag>
  struct bind_column_input_impl<Tag, hana::when<std::is_floating_point<Tag>::value>>
  {
    static void apply(...) = delete;
  };
}

#endif