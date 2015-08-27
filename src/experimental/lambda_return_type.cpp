//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<iostream>

template<typename T>
struct LambdaTraits
{
	using ReturnType = typename LambdaTraits<decltype(&T::operator())>::ReturnType;
};
template<typename ClassType, typename Return, typename... Args>
struct LambdaTraits<Return(ClassType::*)(Args...) const>
{
	using ReturnType = Return;
};

template<typename ReturnType, typename Type, typename Fn>
struct PromiseFulfill;

template<typename Type>
struct Promise
{
	Type value;

	void setValue(Type v)
	{
		value = v;
	}

	template<typename Fn>
	Promise<typename LambdaTraits<Fn>::ReturnType> then(Fn fn)
	{
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

int main()
{
	std::cout << std::endl;
	Promise<int> promise;
	promise.value = 5;
	promise
		.then([](int value) {
			std::cout << value << std::endl;
			return 6;
		})
		.then([](int value) {
			std::cout << value << std::endl;
			return "moo";
		})
		.then([](const char * value) {
			std::cout << value << std::endl;
		})
		;
	std::cout << std::endl;
}
