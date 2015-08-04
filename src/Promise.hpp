#ifndef NBDL_PROMISE_HPP
#define NBDL_PROMISE_HPP

#include<boost/variant.hpp>
#include<vector>
#include<functional>
#include "LambdaTraits.hpp"

namespace nbdl {

template<typename ReturnType, typename Type, typename Fn>
struct PromiseFulfill;

template<typename Type>
struct Promise
{
	//tag dispatch types
	struct Indeterminate {};
	struct Fail {};
	struct ValidationFail {};
	struct NotFound {};

	void resolve(Type v)
	{
		value = v;
		//todo call all callbacks for this resource... hmmm..
	}

	template<typename Fn>
	Promise<typename LambdaTraits<Fn>::ReturnType> then(Fn fn)
	{
		//check if values exist
		//store wrapper for fn	
		return PromiseFulfill<typename LambdaTraits<Fn>::ReturnType, Type, Fn>::call(value, fn);
	}

	private:

	boost::variant<Type> value;
	std::vector<std::function()> callbacks;

};
template<>
struct Promise<void> {};

template<typename ReturnType, typename Type, typename Fn>
struct PromiseFulfill
{
	static Promise<ReturnType> call(Type value, Fn fn)
	{
		Promise<ReturnType> next;
		next.resolve(fn(value));
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
