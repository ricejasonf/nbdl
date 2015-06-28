#ifndef NBDL_PROMISE_HPP
#define NBDL_PROMISE_HPP

#include<boost/optional.hpp>
#include "LambdaTraits.hpp"

namespace nbdl {

template<typename ReturnType, typename Type, typename Fn>
struct PromiseFulfill;

template<typename Type>
struct Promise
{
	boost::optional<Type> value;

	void setValue(Type v)
	{
		value = v;
		//todo call all callbacks for this resource... hmmm..
	}

	template<typename Fn>
	Promise<typename LambdaTraits<Fn>::ReturnType> then(Fn fn)
	{
		//initialize a request
		//todo register callback in static map/vector thingy
		return PromiseFulfill<typename LambdaTraits<Fn>::ReturnType, Type, Fn>::call(value, fn);
	}
};
template<>
struct Promise<void> {};

template<typename ReturnType, typename Type, typename Fn>
struct PromiseFulfill
{
	static Promise<ReturnType> call(Type value, Fn fn)
	{
		Promise<ReturnType> next;
		next.setValue(fn(value));
		return next;
	}
};
template<typename Type, typename Fn>
struct PromiseFulfill<void, Type, Fn>
{
	static Promise<void> call(Type value, Fn fn)
	{
		fn(value);
		return Promise<void>();
	}
};

}//nbdl

#endif
