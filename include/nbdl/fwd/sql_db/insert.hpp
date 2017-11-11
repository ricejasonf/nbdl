//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_SQL_DB_INSERT_HPP
#define NBDL_FWD_SQL_DB_INSERT_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct insert_impl : insert_impl<T, hana::when<true>> { };

  struct insert_fn
  {
    template<typename DbInsert, typename Entity>
    auto operator()(DbInsert, Entity&&) const;

    template<typename DbInsert, typename Entity, typename ForeignKeys>
    auto operator()(DbInsert, Entity&&, ForeignKeys&&) const;
  };

  constexpr insert_fn insert{};
}

#endif
