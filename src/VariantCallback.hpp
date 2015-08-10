#ifndef NBDL_VARIANT_CALLBACK_HPP
#define NBDL_VARIANT_CALLBACK_HPP

#include<type_traits>
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
	static const size_t size = sizeof...(Fns) + 1;
};

template<typename Type,	typename FnSet, class = void>
struct VariantCallbackCaller;

//match if no functions matched and return type is void (do nothing)
template<typename Type,	typename FnSet, class = void>
struct VariantCallbackHelper
{
	//should explode if previous callbacks did not have a void return type
	template<typename... Fns>
	static void call(const Type, Fns...) {}
};
template<typename Type, typename FnSet>
struct VariantCallbackHelper<Type, FnSet,
	typename std::enable_if<(FnSet::size > 0)>::type>
{
	using Fn = typename FnSet::Fn;
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	template<typename... Fns>
	static ReturnType call(const Type value, Fns... fns)
	{
		return VariantCallbackCaller<Type, VariantFnSet<Fns...>>::call(value, fns...);
	}
};

//check the type for a match
template<typename Type,	typename FnSet, class = void>
struct VariantCallbackCallerWithArg
{
	using Fn = typename FnSet::Fn;
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	template<typename... Fns>
	static ReturnType call(const Type value, Fn, Fns... fns)
	{
		//call the next fn because there wasn't a match
		return VariantCallbackHelper<Type, VariantFnSet<Fns...>>::call(value, fns...);
	}
};
template<typename Type, typename FnSet>
struct VariantCallbackCallerWithArg<Type, FnSet,
	typename std::enable_if<
		std::is_same<Type, typename LambdaTraits<typename FnSet::Fn>::template Arg<0>>::value 
	>::type>
{
	using Fn = typename FnSet::Fn;
	using ReturnType = typename LambdaTraits<typename FnSet::Fn>::ReturnType;
	template<typename... Fns>
	static ReturnType call(const Type value, Fn fn, Fns...)
	{
		return fn(value);
	}
};

//match if lambda takes no arguments (catch all)
template<typename Type, typename FnSet>
struct VariantCallbackCaller<Type, FnSet,
	typename std::enable_if<(LambdaTraits<typename FnSet::Fn>::arity == 0)>::type>
{
	using Fn = typename FnSet::Fn;
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	template<typename... Fns>
	static ReturnType call(const Type, Fn fn, Fns...)
	{
		return fn();
	}
};
//match if lambda takes an argument
template<typename Type, typename FnSet>
struct VariantCallbackCaller<Type, FnSet,
	typename std::enable_if<(LambdaTraits<typename FnSet::Fn>::arity == 1)>::type>
{
	using Fn = typename FnSet::Fn;
	using ReturnType = typename LambdaTraits<Fn>::ReturnType;
	template<typename... Fns>
	static ReturnType call(const Type value, Fn fn, Fns... fns)
	{
		return VariantCallbackCallerWithArg<Type, VariantFnSet<Fn, Fns...>>::call(value, fn, fns...);
	}
};

}//details

/*
   The VariantCallback is meant to be dynamic for the user so any order
   should be acceptable and omitted types should default to do nothing.
*/
struct VariantCallback
{
	template<typename Type, typename Fn1, typename... Fns>
	static typename LambdaTraits<Fn1>::ReturnType call(const Type value, Fn1 fn, Fns... fns)
	{
		return details::VariantCallbackHelper<Type, details::VariantFnSet<Fn1, Fns...>>::call(value, fn, fns...);
	}
};

}//nbdl

#endif
