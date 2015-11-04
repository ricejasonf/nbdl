//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDERS_JSONCPP_VALIDATION_ERRORS_HPP
#define NBDL_BINDERS_JSONCPP_VALIDATION_ERRORS_HPP

#include<jsoncpp/json/json.h>

namespace nbdl {
namespace binders {
namespace jsoncpp {

class ValidationErrors
{
	Json::Value &jsonVal;

	public: 

	ValidationErrors(Json::Value &value)
		: jsonVal(value) {}

	void addError(std::string name, ErrorToken token)
	{
		jsonVal[name] = toString(token);
	}

	template<typename Functor>
	void validateChild(const std::string name, Functor fn)
	{
		Json::Value obj = Json::Value(Json::objectValue);
		ValidationErrors errors(obj);
		fn(errors);
		jsonVal[name] = obj;
	}
};

}//jsoncpp
}//binders
}//nbdl
#endif
