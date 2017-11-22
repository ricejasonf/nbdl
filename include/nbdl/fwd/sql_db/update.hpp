//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_SQL_DB_UPDATE_HPP
#define NBDL_FWD_SQL_DB_UPDATE_HPP

#include <boost/hana/core/when.hpp>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;

  template<typename T, typename = void>
  struct update_impl : update_impl<T, hana::when<true>> { };

  struct update_fn
  {
    template<typename DbInsert, typename Entity, typename Keys>
    void operator()(DbInsert, Entity&&, Keys&&) const;
  };

  constexpr update_fn update{};
}

#endif
