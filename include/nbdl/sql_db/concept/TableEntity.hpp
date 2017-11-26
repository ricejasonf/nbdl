//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_CONCEPT_TABLE_ENTITY_HPP
#define NBDL_SQL_DB_CONCEPT_TABLE_ENTITY_HPP

#include <nbdl/concept/Entity.hpp>
#include <nbdl/concept/EntityContainer.hpp>
#include <nbdl/fwd/sql_db/concept/TableEntity.hpp>

#include <boost/hana/core/default.hpp>
#include <type_traits>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;

  // TableEntity represents a type that purports
  // database operation support for ops such as
  // select, insert, update, delete
  // This is currently used to distinguish between
  // types that represent a column or a row(s)
  // TODO: This could stand to be improved or broken apart
  // into Selectable, Insertable, Deletable, Updateable
  // concepts
  // See also: is_multirow<T> (traits.hpp)
  template <typename T, typename>
  struct TableEntity
    : std::false_type
  { };

  template <typename T>
  struct TableEntity<T, std::enable_if_t<nbdl::Entity<T>::value or nbdl::EntityContainer<T>::value>>
    : std::true_type
  { };
}

#endif
