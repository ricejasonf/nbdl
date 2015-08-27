//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPL_NUMBER_SEQUENCE
#define NBDL_MPL_NUMBER_SEQUENCE

namespace nbdl {
namespace mpl {

template<int... sequence>
struct NumberSequence {};

template<int n, int... sequence>
struct CreateNumberSequence
{
	using Type = typename CreateNumberSequence<n - 1, n - 1, sequence...>::Type;
};
template<int... sequence>
struct CreateNumberSequence<0, sequence...>
{
	using Type = NumberSequence<sequence...>;
};

}//mpl
}//nbdl

#endif
