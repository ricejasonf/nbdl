//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_METASTRUCT_HPP
#define NBDL_MPDEF_METASTRUCT_HPP
namespace mpdef {

struct MetastructTag {};

}//mpdef

namespace boost { namespace hana {

  template<>
  struct equal_impl<mpdef::MetastructTag, mpdef::MetastructTag>
  {
    template<typename T, typename U>
    static constexpr auto apply(T, U)
    {
      return hana::false_c;
    }

    template<typename T>
    static constexpr auto apply(T, T)
    {
      return hana::true_c;
    }
  };

}}//boost::hana

#endif
