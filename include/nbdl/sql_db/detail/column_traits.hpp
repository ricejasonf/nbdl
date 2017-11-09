//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_DETAIL_COLUMN_TRAITS_HPP
#define NBDL_SQL_DB_DETAIL_COLUMN_TRAITS_HPP

#include <nbdl/concept/Entity.hpp>
#include <nbdl/entity_members.hpp>
#include <nbdl/fwd/sql_db/column_traits.hpp>

#include <sqlpp11/type_traits.h>
#include <type_traits>
#include <utility>

namespace nbdl::sql_db::detail
{
  template <typename T, typename = void>
  struct column_traits_impl;

  template <typename T>
  column_traits_impl<T, std::enable_if_t<nbdl::String<T>::value>>
  {
    using type = sqlpp::make_traits<:q
  };

}

#endif
