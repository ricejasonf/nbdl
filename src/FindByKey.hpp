//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FIND_BY_KEY_HPP
#define NBDL_FIND_BY_KEY_HPP

#include<boost/hana.hpp>
#include<utility>

namespace nbdl {

namespace hana = boost::hana;

struct FindByKey
{
  /* doesn't work with references
  template<typename Xs, typename Key>
  constexpr decltype(auto) operator()(Xs&& xs, const Key& key) const
  {
    return hana::second(hana::find_if(static_cast<Xs&&>(xs), 
      hana::on(hana::equal.to(key), hana::first)).value()); 
  }
  */
  template<typename Xs, typename Key>
  constexpr decltype(auto) operator()(Xs&& xs, const Key& key) const
  {
    using Pack = typename hana::detail::make_pack<Xs>::type;
    using Pred = decltype(hana::compose(hana::equal.to(key), hana::first));
    constexpr std::size_t index = hana::detail::index_if<Pred, Pack>::value;
    return hana::second(hana::at_c<index>(static_cast<Xs&&>(xs)));
  }
};

constexpr FindByKey findByKey{};

}//nbdl

#endif
