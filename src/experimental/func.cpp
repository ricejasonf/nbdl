//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<iostream>
#include<string>

template<class SuccessFn, class ValidationFailFn, class FailFn>
class SomethingCaller
{
	SuccessFn successFn;
	ValidationFailFn vfailFn;
	FailFn failFn;

	public: 
	
	SomethingCaller(SuccessFn success, ValidationFailFn vfail, FailFn fail) :
		successFn(success),
		vfailFn(vfail),
		failFn(fail) 
	{}

	void success()
	{
		successFn();
	}

	void validationFailed()
	{
		vfailFn();
	}

	void failed(std::string msg)
	{
		failFn(msg);
	}
};

class PathFactory
{
	
}

template<class SuccessFn, class ValidationFailFn, class FailFn>
static SomethingCaller<SuccessFn, ValidationFailFn, FailFn> create(SuccessFn success, ValidationFailFn vfail, FailFn fail)
{
	return SomethingCaller<SuccessFn, ValidationFailFn, FailFn>(success, vfail, fail);	
}

int main()
{
	auto speak = []{ return "moo"; };

	auto callbackObj = create(
		[speak]() { 
			std::cout << "The cow says " << speak() << std::endl;
		},
		[]() { 
			std::cout << "Validation failed." << std::endl;
		},
		[](std::string msg) { 
			std::cout << msg << std::endl;
		});

	callbackObj.success();
	callbackObj.validationFailed();
	callbackObj.failed("An error has occurred.");
}
