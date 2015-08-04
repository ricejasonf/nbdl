#ifndef NBDL_VARIANT_CALLBACK_HPP
#define NBDL_VARIANT_CALLBACK_HPP

#include<typetraits>
#include<tuple>
#include "LambdaTraits.hpp"

namespace nbdl {

namespace details {

typename<typename Type,	typename Fn, typename... Fns, class = void>
struct VariantCallbackCaller {};
//match if lambda takes no arguments (catch all)
typename<typename Type, typename Fn, typename... Fns>
struct VariantCallbackCaller<Type, Fn, Fns...>
	typename std::enable_if<LambdaTraits<Fn>::arity == 0>::type>
{
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	static ReturnType call(Type value, Fn fn, Fns... fns)
	{
		return fn();
	}
};
//match if lambda takes an argument
typename<typename Type, typename Fn, typename... Fns>
struct VariantCallbackCaller<Type, Fn, Fns...>
	typename std::enable_if<LambdaTraits<Fn>::arity == 1>::type>
{
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	static ReturnType call(Type value, Fn fn, Fns... fns)
	{
		return VariantCalbackCallerWithArg<Type, Fn, Fns...>::call(value, fn, fns...);
	}
};
//match if no functions matched and return type is void (do nothing)
typename<typename Type, typename... Fns, class = void>
struct VariantCallbackCaller<Type, Fns...>
{
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	static_assert(std::is_same<ReturnType, void>::value,
		"No functions matched and a return value is expected.");
	static void call(Type value, Fns... fns) {}
};

//check the type for a match
typename<typename Type,	typename Fn, typename... Fns, class = void>
struct VariantCallbackCallerWithArg
{
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	static ReturnType call(Type value, Fn fn, Fns... fns)
	{
		//call the next fn because there wasn't a match
		return VariantCallbackCaller<Type, Fns...>::call(value, fns...);
	}
};
typename<typename Type, typename Fn, typename... Fns>
struct VariantCallbackCallerWithArg<Type, Fn, Fns...>
	typename std::enable_if< 
		std::is_same<Type, typename LambdaTraits<Fn>::Arg<0>>::value 
	>::type>
{
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	static ReturnType call(Type value, Fn fn, Fns... fns)
	{
		return fn(value);
	}
};

}//details

/*
   The VariantCallback is meant to be dynamic for the user so any order
   should be acceptable and omitted types should default to do nothing.
*/
struct VariantCallback
{
	template<typename Type, typename... Fns>
	static ReturnType call(Type value, Fns... fns)
	{
		return details::VariantCallbackCaller<Type, Fns>::call(value, fns...);
	}
};

}//nbdl

#endif
