//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_COLLECT_SETTINGS_HPP
#define NBDL_MPDEF_COLLECT_SETTINGS_HPP

#include<boost/hana.hpp>
#include "../def/meta.hpp"

namespace nbdl_def_meta {

namespace hana = boost::hana;

struct CollectSettings
{
  template<typename State, typename X>
  auto helper(State const& state, X const& x, hana::map_tag)
  {
    hana::unpack(hana::keys(state),
      hana::make_map ^hana::on^
      hana::demux(hana::maybe)
      (
        hana::partial(hana::at_key, x),
        hana::always(hana::just),
        hana::partial(hana::find, x)
      )
    );
  }

  template<typename State, typename X>
  constexpr auto helper(State&& state, X&& x, hana::tuple_tag)
  {
    return std::forward<State>(state); 
  }

  template<typename State, typename X>
  constexpr auto operator()(State&& state, X&& x)
  {
    return decltype(
      helper(std::forward<State>(state), std::forward<X>(x), hana::tag_of_t<x>{})
    ){};
  }
};
constexpr CollectSettings collectSettings{};

template<typename... Tag>
constexpr auto withSettings(Tag&&... tag)
{
  return hana::make_map(hana::make_pair(std::forward<Tag>(tag), hana::nothing)...);
}

}//nbdl_def_meta
#endif
