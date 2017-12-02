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
#include <nbdl/fwd/sql_db/maria_db.hpp>

#include <boost/hana/bool.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/if.hpp>
#include <type_traits>
#include <utility>

namespace nbdl::sql_db::mariadb
{
  namespace hana = boost::hana;

  template <typename Tag, bool condition>
  constexpr auto column_type_name_impl<Tag, hana::when<condition>> = 1;

  namespace detail
  {
    template <typename T>
    using column_type_name = std::decay_t<decltype(column_type_name_impl<hana::tag_of_t<T>>)>;
  }

  // String

  template <typename Tag>
  constexpr auto column_type_name_impl<Tag, hana::when<nbdl::String<Tag>::value>> = "VARCHAR"_s;

  // DynamicBuffer

  template <typename Tag>
  constexpr auto column_type_name_impl<Tag, hana::when<
    nbdl::DynamicBuffer<Tag>::value and not nbdl::String<Tag>::value>> = "BLOB"_s;

  // Bool

  // note: std::is_unsigned<bool> is true
  template <>
  constexpr auto column_type_name_impl<bool> = "BOOL"_s;

  // Integral

  template <typename Tag>
  constexpr auto column_type_name_impl<Tag, hana::when<
    std::is_integral<Tag>::value and not std::is_same<Tag, bool>::value
  >> = hana::plus(
    hana::if_(hana::bool_c<sizeof(Tag) == 8>, "BIGINT"_S,
    hana::if_(hana::bool_c<sizeof(Tag) == 4>, "INT"_S,
    hana::if_(hana::bool_c<sizeof(Tag) == 2>, "SMALLINT"_s
    hana::if_(hana::bool_c<sizeof(Tag) == 1>, "TINYINT"_s
      void
    ))))
  , 
    hana::if_(std::is_unsigned<Tag>{}, " UNSIGNED"_s, ""_s)
  );

  // FloatingPoint (float, double, long double, double double w/cheese)

  template <typename Tag>
  constexpr auto column_type_name_impl<Tag, hana::when<std::is_floating_point<Tag>::value>>
    = hana::if_(hana::bool_c<sizeof(Tag) == 4>, "FLOAT(32)"_s,
      hana::if_(hana::bool_c<sizeof(Tag) == 8>, "FLOAT(64)"_s,
        "FLOAT(80)"_s // <-- w/cheese
      ));
}

#endif
