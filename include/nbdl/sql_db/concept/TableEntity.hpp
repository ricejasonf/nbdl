//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_CONCEPT_TABLE_ENTITY_HPP
#define NBDL_SQL_DB_CONCEPT_TABLE_ENTITY_HPP

#include<nbdl/fwd/sql_db/concept/TableEntity.hpp>

#include<boost/hana/core/default.hpp>
#include<type_traits>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;

  template<typename T>
  struct TableEntity
  {
    using Tag = hana::tag_of_t<T>;
    static constexpr bool value = (
        !hana::is_default<nbdl::sql_db::insert_impl<Tag>>::value
        // TODO create_table, update, delete_ ...
    );
  };
}

#endif
