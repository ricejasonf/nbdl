#ifndef NBDL_UNIQUE_HPP
#define NBDL_UNIQUE_HPP

#include<type_traits>
#include<tuple>
#include "TupleGetByType.hpp"

namespace nbdl {
namespace mpl {

namespace details {

template<typename T, typename Tuple, class = void>
struct UniquePrepend
{
	using Type = Tuple;
};

template<typename T, typename... Ts>
struct UniquePrepend<T, std::tuple<Ts...>,
	typename std::enable_if<(!HasType<T, std::tuple<Ts...>>::value)>::type>
{
	using Type = std::tuple<T, Ts...>;
};

}//details

template<typename Tuple>
struct Unique {};

template<typename... Ts>
struct Unique<std::tuple<Ts...>>
{
	using Type = std::tuple<>;
};

template<typename T, typename... Ts>
struct Unique<std::tuple<T, Ts...>>
{
	using Type = typename details::UniquePrepend<T, typename Unique<std::tuple<Ts...>>::Type>::Type;
};

}//mpl
}//nbdl

#endif
