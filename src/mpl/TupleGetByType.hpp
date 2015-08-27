//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPL_TUPLE_GET_BY_TYPE
#define NBDL_MPL_TUPLE_GET_BY_TYPE

#include<tuple>

namespace nbdl {
namespace mpl {

template<typename T, typename Tuple>
struct HasType
{
	static constexpr bool value = false;
};

template<typename T, typename... Ts>
struct HasType<T, std::tuple<T, Ts...>>
{
	static constexpr bool value = true;
};

template<typename T, typename U, typename... Ts>
struct HasType<T, std::tuple<U, Ts...>>
{
	static constexpr bool value = HasType<T, std::tuple<Ts...>>::value;
};

template<typename T, typename Tuple>
struct TupleIndexByType;

template<typename T, typename... Ts>
struct TupleIndexByType<T, std::tuple<T, Ts...>>
{
	static constexpr std::size_t value = 0;
};

template<typename T, typename U, typename... Ts>
struct TupleIndexByType<T, std::tuple<U, Ts...>>
{
	using Next = TupleIndexByType<T, std::tuple<Ts...>>;
	static constexpr std::size_t value = Next::value + 1;
};

template<typename T, typename Tuple>
struct TupleGetByType
{
	//c++11 std::get<std::tuple> is not yet constexpr
	static T& get(Tuple& tuple)
	{
		return std::get<TupleIndexByType<T, Tuple>::value>(tuple);
	}
};

}//mpl
}//nbdl

#endif
