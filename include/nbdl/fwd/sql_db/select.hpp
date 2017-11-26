//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_SQL_DB_SELECT_HPP
#define NBDL_FWD_SQL_DB_SELECT_HPP

#include <nbdl/sql_db/statement/detail/table_alias_list.hpp>

#include <boost/hana/core/when.hpp>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;

  template <typename T, typename = void>
  struct select_impl : select_impl<T, hana::when<true>> { };

  template <typename Entity>
  struct select_fn
  {
    template <typename DbSelect, typename Keys>
    auto operator()(DbSelect, Keys&&) const;
  };

  template <typename Entity>
  constexpr select_fn<Entity> select{};

  template <typename Entity>
  using select_primary_key_name = statement::detail::select_primary_key_name<Entity>;

  template <typename Entity, typename Member1, typename ...MemberN>
  using select_nested_key_name = statement::detail::select_nested_key_name<Member1, MemberN...>;
}

#endif
