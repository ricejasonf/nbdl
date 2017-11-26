//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_STATEMENT_DETAIL_TABLE_ALIAS_LIST_HPP
#define NBDL_SQL_DB_STATEMENT_DETAIL_TABLE_ALIAS_LIST_HPP

#include <mpdef/list.hpp>
#include <nbdl/sql_db/detail/string_sum.hpp>
#include <nbdl/sql_db/traits.hpp>

#include <boost/hana/plus.hpp>
#include <boost/hana/string.hpp>
#include <boost/mp11/list.hpp>

/* SCRATCH
  foo {
    bar foo_bar;
    struct bar { baz bar_baz; } foo_bar;
    struct boo { } foo_boo;
  }

  Breadth first since the implementation is simpler
  with aliases

  foo
  foo.foo_bar
  foo.foo_boo
  foo.foo_bar.bar_baz
 * */

namespace nbdl::sql_db::statement::detail
{
  namespace hana = boost::hana;
  using namespace boost::mp11;
  using namespace boost::hana::literals;

  using sql_db::detail::string_sum;
  using sql_db::detail::string_sum_join;
  using sql_db::detail::identifier_quote;

  using alias_separator = hana::string<'.'>;
  using table_column_separator = hana::string<'.'>;

  template <template<class...> class F, typename Arg1, typename L>
  using apply_one = mp_apply<F, mp_push_front<L, Arg1>>;

  template <typename Prefix, typename ...Name>
  using concat_table_alias = decltype(
    string_sum_join<alias_separator>(Prefix{}, Name{}...)
  );

  template <typename IsEmpty>
  struct member_alias_list_impl
  {
    template <typename Prefix, typename ...Member>
    using f = mpdef::list<>;
  };

  template <>
  struct member_alias_list_impl<mp_bool<false>>
  {
    template <typename Prefix, typename ...Member>
    using f = mp_append<
      mpdef::list<mpdef::list<
        Member
      , concat_table_alias<Prefix, member_name<Member>>
      , Prefix
      >...>
    , mp_append<
        apply_one<
          member_alias_list_impl<mp_empty<members_child<typename Member::member_type>>>::template f
        , concat_table_alias<Prefix, member_name<Member>>
        , members_child<typename Member::member_type>
        >...
      >
    >;
  };

  template <typename Prefix, typename ...Member>
  using member_alias_list = typename member_alias_list_impl<mp_false>::template f<Prefix, Member...>;

  // Returns a flattened list of aliases for "child" members
  // It does not include the Entity itself.
  // -> list<pair<Member, Name, ParentName>...>
  template <typename Entity>
  using table_alias_list = apply_one<
    member_alias_list
  , table_name<Entity>
  , members_child<Entity>
  >;

  /*
   * aliased_column_names
   */
  template <typename TableAlias, typename Name>
  using concat_table_alias_member_name = decltype(
    sql_db::detail::string_sum(
      identifier_quote<TableAlias>{}
    , table_column_separator{}
    , identifier_quote<Name>{}
    )
  );

  template <typename TableAlias, typename ...Member>
  using aliased_column_name_list = mp_append<mpdef::list<
    mpdef::pair<
      Member
    , concat_table_alias_member_name<TableAlias, nbdl::member_name<Member>>
    >...
  >>;

  template <typename IsEmpty>
  struct aliased_column_names_impl
  {
    template <typename ...>
    using f = mpdef::list<>;
  };

  template <>
  struct aliased_column_names_impl<mp_bool<false>>
  {
    template <typename Prefix, typename ...Member>
    using f = mp_append<
      apply_one<
        aliased_column_name_list
        , concat_table_alias<Prefix, member_name<Member>>
      , members_value<typename Member::member_type>
      >...
    , mp_append<
        apply_one<
          aliased_column_names_impl<mp_empty<members_child<typename Member::member_type>>>::template f
        , concat_table_alias<Prefix, member_name<Member>>
        , members_child<typename Member::member_type>
        >...
      >
    >;
  };

  // Returns a flattened list of all value column names prefixed with their
  // respective table alias
  // -> list<pair<Member, Alias>...>
  template <typename Entity>
  using aliased_column_names = mp_append<
    apply_one<aliased_column_name_list, table_name<Entity>, members_value<Entity>>
  , mp_append<
      apply_one<
        aliased_column_names_impl<mp_empty<members_child<Entity>>>::template f
      , table_name<Entity>
      , members_child<Entity>
      >
    >
  >;

  //
  // columns_clause
  //
  template <typename Entity>
  using columns_clause = decltype(
    hana::unpack(
      mp_transform<mp_second, statement::detail::aliased_column_names<Entity>>{}
    , string_sum_join<hana::string<','>>
    )
  );

  //
  // join_clauses
  // Row is list<Member, TableAlias, ParentAlias>
  //
  // JOIN "foo" AS "bar.bar_foo" ON "bar"."bar_foo_id" = "bar.bar_foo"."foo_id"
  template <typename R>
  using join_clause = decltype(
    sql_db::detail::string_sum(
      " JOIN "_s
    , identifier_quote<table_name<typename mp_first<R>::member_type>>{}
    , " AS "_s
    , identifier_quote<mp_second<R>>{}
    , " ON "_s
    , concat_table_alias_member_name<mp_third<R>, member_key_name<mp_first<R>>>{}
    , " = "_s
    , concat_table_alias_member_name<mp_second<R>, table_key_name<typename mp_first<R>::member_type>>{}
    )
  );

  template <typename Entity>
  using join_clauses = decltype(
    hana::unpack(
      mp_transform<join_clause, table_alias_list<Entity>>{}
    , string_sum_join<hana::string<' '>>
    )
  );

  //
  // select_key_name
  //

  template <typename ...T>
  using tail = mp_at_c<mpdef::list<T...>, sizeof...(T) - 1>;

  template <typename Entity, typename ...Member>
  using select_nested_key_name = decltype(
    sql_db::detail::string_sum(
      identifier_quote<concat_table_alias<table_name<Entity>, member_name<Member>...>>{}
    , table_column_separator{}
    , identifier_quote<table_key_name<tail<Member...>>>{}
    )
  );

  template <typename Entity>
  using select_primary_key_name = decltype(
    sql_db::detail::string_sum(
      identifier_quote<table_name<Entity>>{}
    , table_column_separator{}
    , identifier_quote<table_key_name<Entity>>{}
    )
  );
}

#endif
