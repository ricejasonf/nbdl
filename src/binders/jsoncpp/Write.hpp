//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDERS_JSONCPP_WRITE_HPP
#define NBDL_BINDERS_JSONCPP_WRITE_HPP

#include<string>
#include<jsoncpp/json/json.h>

namespace nbdl {
namespace binders {
namespace jsoncpp {

class Write
{
	Json::Value &jsonVal;

	public:

	Write(Json::Value &value) :
		jsonVal(value)
	{}

	template<typename T>
	void bindMember(const std::string name, T & field)
	{ 
		jsonVal[name] = field; 
	}

	template<class BinderFn>
	void bindEntity(const std::string name, BinderFn bind)
	{
		auto obj = Json::Value(Json::objectValue);
		Write writer(obj);
		bind(writer);
		jsonVal[name] = obj;
	}

};

}//jsoncpp
}//binders
}//nbdl

#endif
