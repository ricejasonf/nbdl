//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPL_REVERSE_HPP
#define NBDL_MPL_REVERSE_HPP

#include "NumberSequence.hpp"

namespace nbdl {
namespace mpl {

namespace details {

template<typename Tuple, typename Sequence>
struct ReverseTuple;

template<typename Tuple, int... sequence>
struct ReverseTuple<Tuple, NumberSequence<sequence...>>
{
	using Type = std::tuple<typename std::tuple_element<(sizeof...(sequence) - 1 - sequence), Tuple>::type...>;
}; 

}//details

template<typename Tuple>
struct Reverse;

template<typename... Ts>
struct Reverse<std::tuple<Ts...>>
{
	using Tuple = std::tuple<Ts...>;
	using Sequence = typename CreateNumberSequence<std::tuple_size<Tuple>::value>::Type;
	using Type = typename details::ReverseTuple<Tuple, Sequence>::Type;
};

}//mpl
}//nbdl

#endif
