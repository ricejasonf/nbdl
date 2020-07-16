//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_TRAITS_HPP
#define NBDL_SQL_DB_TRAITS_HPP

#include <nbdl/entity_members.hpp>
#include <nbdl/key.hpp>
#include <nbdl/concept/EntityContainer.hpp>
#include <nbdl/sql_db/concept/TableEntity.hpp>

#include <boost/mp11/algorithm.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/plus.hpp>
#include <boost/hana/string.hpp>
#include <boost/mp11/function.hpp>
#include <type_traits>

namespace nbdl::sql_db
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  // primary key

  template <typename Entity>
  struct primary_key_impl
  {
    using type = nbdl::key<Entity>;
  };

  template <typename Entity>
  using primary_key = typename primary_key_impl<std::decay_t<Entity>>::type;

  // table name

  template <typename Entity>
  struct table_name_impl
  {
    using type = nbdl::entity_name<Entity>;
  };

  template <typename Entity>
  using table_name = typename table_name_impl<std::decay_t<Entity>>::type;

  // is_multi_row


  template <typename T>
  constexpr bool is_multirow_impl = false;

  template <EntityContainer T>
  constexpr bool is_multirow_impl<T> = true;

  template <typename T>
  constexpr bool is_multirow = is_multirow_impl<std::decay_t<T>>;

  // member stuff

  template <typename T>
  using is_member_child = mp_bool<
     TableEntity<typename T::member_type>
  && !is_multirow<typename T::member_type>
  >;

  template <typename T>
  using is_member_multirow = mp_bool<
     TableEntity<typename T::member_type>
  && is_multirow<typename T::member_type>
  >;

  template <typename T>
  using is_member_value = mp_not<mp_bool<TableEntity<typename T::member_type>>>;

  template <typename Entity>
  using members_child = mp_copy_if<nbdl::entity_members_t<Entity>, is_member_child>;

  template <typename Entity>
  using members_multirow = mp_copy_if<nbdl::entity_members_t<Entity>, is_member_multirow>;

  template <typename Entity>
  using members_value = mp_copy_if<nbdl::entity_members_t<Entity>, is_member_value>;

  template <typename T>
  using table_key_name = decltype(hana::plus(table_name<T>{}, hana::string<'_', 'i', 'd'>{}));

  template <typename T>
  using member_key_name = decltype(hana::plus(nbdl::member_name<T>{}, hana::string<'_', 'i', 'd'>{}));
}

#endif
