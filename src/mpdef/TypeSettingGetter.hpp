//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_CREATE_TYPE_SETTING_GETTER_HPP
#define NBDL_MPDEF_CREATE_TYPE_SETTING_GETTER_HPP

#include<boost/hana.hpp>
#include "../Make.hpp"
#include "../def/meta.hpp" //FindSetting.hpp

namespace nbdl_def_meta {

namespace hana = boost::hana;

template<typename TypeMap, typename DefaultType>
struct TypeSettingGetter
{

  TypeMap type_map;
  DefaultType default_type;

  TypeSettingGetter(TypeMap m, DefaultType t) :
    type_map(m),
    default_type(t)
  {}

  template<typename Xs>
  constexpr auto operator()(Xs&& xs)
  {
    return (
      findSetting(std::forward<Xs>(xs))
      | hana::reverse_partial(hana::find_if, hana::always(hana::true_))
      | hana::partial(hana::find_if, type_map)
    ).value_or(default_type);
  }
  
};
constexpr auto makeTypeSettingGetter = nbdl::Make<TypeSettingGetter>;

}//nbdl_def_meta
