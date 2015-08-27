//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDERS_EMSCRIPTEN_VALIDATIONERRORS_HPP
#define NBDL_BINDERS_EMSCRIPTEN_VALIDATIONERRORS_HPP

#include<string>
#include<emscripten/val.h>

namespace nbdl {
namespace binders {
namespace emscripten {

namespace emscripten = ::emscripten;

class ValidationErrors
{
	public:

	emscripten::val emVal;

	using NameFormat = DefaultNameFormat;

	ValidationErrors() :
		emVal(emscripten::val::object()) {}

	void addError(std::string name, ErrorToken token)
	{
		emVal.set(name, toString(token));
	}

	template<typename Functor>
	void validateChild(const std::string name, Functor fn)
	{
		ValidationErrors errors;
		fn(errors);
		emVal.set(name, errors.emVal);
	}
};

}//emscripten
}//binders
}//nbdl
#endif
