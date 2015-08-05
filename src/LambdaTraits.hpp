#ifndef NBDL_LAMBDA_TRAITS_HPP
#define NBDL_LAMBDA_TRAITS_HPP

#include<tuple>

namespace nbdl {

template<typename T>
struct LambdaTraits
{
	using ReturnType = typename LambdaTraits<decltype(&T::operator())>::ReturnType;
};
template<typename ClassType, typename Return, typename... Args>
struct LambdaTraits<Return(ClassType::*)(Args...) const>
{
	using ReturnType = Return;

	template<std::size_t i>
	using Arg = typename std::tuple_element<i, std::tuple<Args...>>::type;

	static const unsigned arity = sizeof...(Args);
};

}//nbdl

#endif
