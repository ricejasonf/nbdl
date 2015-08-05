#ifndef NBDL_VARIANT_CALLBACK_HPP
#define NBDL_VARIANT_CALLBACK_HPP

#include<type_traits>
#include<tuple>
#include "LambdaTraits.hpp"

namespace nbdl {

namespace details {

template<typename... Fns>
struct VariantFnSet
{
	static const size_t size = sizeof...(Fns);
};
template<typename Fn1, typename... Fns>
struct VariantFnSet<Fn1, Fns...>
{
	using Fn = Fn1;
	using Next = VariantFnSet<Fns...>;
	static const size_t size = sizeof...(Fns);
};

template<typename Type,	typename FnSet, class = void>
struct VariantCallbackCaller {};

//match if no functions matched and return type is void (do nothing)
template<typename Type,	typename FnSet, class = void>
struct VariantCallbackHelper
{
	//should explode if previous callbacks did not have a void return type
	template<typename... Fns>
	static void call(Type value, Fns... fns) {}
};
template<typename Type, typename FnSet>
struct VariantCallbackHelper<Type, FnSet,
	typename std::enable_if<(FnSet::size > 0)>::type>
{
	template<typename... Fns>
	static void call(Type value, Fns... fns)
	{
		return VariantCallbackCaller<Type, FnSet>::call(value, fns...);
	}
};

//match if lambda takes no arguments (catch all)
template<typename Type, typename FnSet>
struct VariantCallbackCaller<Type, FnSet>
	typename std::enable_if<LambdaTraits<typename FnSet::Fn>::arity == 0>::type>
{
	using Fn = typename FnSet::Fn;
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	template<typename... Fns>
	static ReturnType call(Type value, Fn fn, Fns... fns)
	{
		return fn();
	}
};
//match if lambda takes an argument
template<typename Type, typename FnSet>
struct VariantCallbackCaller<Type, FnSet>
	typename std::enable_if<LambdaTraits<typename FnSet::Fn>::arity == 1>::type>
{
	using Fn = typename FnSet::Fn;
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	template<typename... Fns>
	static ReturnType call(Type value, Fn fn, Fns... fns)
	{
		return VariantCalbackCallerWithArg<Type, FnSet<Fns...>>::call(value, fn, fns...);
	}
};

//check the type for a match
template<typename Type,	typename FnSet, class = void>
struct VariantCallbackCallerWithArg
{
	using Fn = typename FnSet::Fn;
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	template<typename... Fns>
	static ReturnType call(Type value, Fn fn, Fns... fns)
	{
		//call the next fn because there wasn't a match
		return VariantCallbackHelper<Type, FnSet<Fns...>>::call(value, fns...);
	}
};
template<typename Type, typename FnSet>
struct VariantCallbackCallerWithArg<Type, FnSet>
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
		return details::VariantCallbackHelper<Type, details::VariantFnSet<Fns...>>::call(value, fns...);
	}
};

}//nbdl

#endif
