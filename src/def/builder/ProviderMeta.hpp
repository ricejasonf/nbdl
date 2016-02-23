//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_PROVIDER_META_HPP
#define NBDL_DEF_PROVIDER_META_HPP

#include<mpdef/List.hpp>
#include<mpdef/MPDEF_METASTRUCT.hpp>
#include<mpdef/MakeMetastruct.hpp>

#include<boost/hana.hpp>

namespace nbdl_def {
namespace builder {

namespace hana = boost::hana;

MPDEF_METASTRUCT(
  ProviderMeta
  , provider 
  , name 
  , accessPoints
);
constexpr auto makeProviderMeta = mpdef::makeMetastruct<ProviderMeta>;

// this is lame i know
struct GetAccessPointsFromProvidersMeta
{
  struct Helper
  {
    template<typename T>
    constexpr auto operator()(T t) const
    {
      return t.accessPoints();
    };
  };

  template<typename S>
  constexpr auto operator()(S s) const
  {
    return hana::flatten(hana::unpack(s, mpdef::make_list ^hana::on^ Helper{}));
  }
};
constexpr GetAccessPointsFromProvidersMeta getAccessPointsFromProvidersMeta{};

}//builder
}//nbdl_def
#endif
