//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_SHOW_IF_HPP
#define NBDL_MPDEF_SHOW_IF_HPP

#include<boost/hana.hpp>

namespace mpdef {

namespace hana = boost::hana;

struct ShowIf
{
  template<typename ...T>
  struct Show { };

  struct DontShow { using foo = void; };

  template<typename Cond, typename ...T>
  constexpr void operator()(Cond, T&&...) const
  {
     using Foo = typename decltype(hana::if_(Cond{}, Show<T...>{}, DontShow{}))::foo; 
  }
};
constexpr ShowIf show_if{};

}//mpdef
#endif
