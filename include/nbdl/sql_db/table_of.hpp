//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_SQL_DB_ACTION_HPP
#define NBDL_SQL_DB_ACTION_HPP


#include <boost/hana.hpp>
#include <sqlpp11/something.h>
#include <utility>

namespace nbdl::sql_db
{
  template <typename T, typename = void>
  struct table_of_impl;

  template <typename EntityMember, typename = void>
  struct column_traits_impl;

  namespace detail
  {
    template <typename HanaString>
    struct char_sequence
    {
      static constexpr char* char_ptr()
      {
        return HanaString{}.c_str();
      }
    };

    template <typename EntityMember>
    struct entity_member_column_spec
    {
      struct _alias_t 
      {
        using _name_t = char_sequence<member_name<EntityMember>>;

        template <typename Column>
        struct member_t
        {
          Column column;

          auto& operator()() { return column; }
          auto const& operator()() const { return column; }
        };
      };

      using _traits = typename column_traits_impl<EntityMember>::type;
    };

    template <typename Entity, mpdef::list<typename ...EntityMember>>
    struct entity_table
    {
      using type = sqlpp::table_t<table_impl<Entity>
                                , entity_member_column_spec<EntityMember>...
                                >;
    };
  }

  template <typename T>
  struct table_impl<T, hana::when<nbdl::Entity<T>::value>>
    : sqlpp::table_t<table_impl<T>
                     , column_spec<
  {
    using type = sqlpp::table_t<
  };

  template <typename T>
  constexpr auto table_of = table_of_impl
}

#endif
