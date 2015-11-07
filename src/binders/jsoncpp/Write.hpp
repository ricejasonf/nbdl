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
	void bindMember(const std::string name, const T& field)
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

  void bindVariantEmpty(const std::string name, const int type_id)
  {
    jsonVal[name] = type_id;
  }

	template<class BinderFn>
  void bindVariantEntity(const std::string name, const int type_id, BinderFn bind)
  {
		auto obj = Json::Value(Json::objectValue);
		Write writer(obj);
		bind(writer);

	  auto array = Json::Value(Json::arrayValue);
    array.append(type_id);
    array.append(obj);
    jsonVal[name] = array;
  }

  template<typename Value_>
  void bindVariantValue(const std::string name, const int type_id, const Value_& value)
  {
	  auto array = Json::Value(Json::arrayValue);
    array.append(type_id);
    array.append(value);
    jsonVal[name] = array;
  }

};

}//jsoncpp
}//binders
}//nbdl

#endif
