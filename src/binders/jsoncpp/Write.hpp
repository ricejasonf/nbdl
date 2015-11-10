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

  Write bindVariantEntity(const std::string name, const int type_id)
  {
		auto obj = Json::Value(Json::objectValue);
		Write writer(obj);

	  auto array = Json::Value(Json::arrayValue);
    array.append(type_id);
    array.append(obj);
    jsonVal[name] = array;

    return writer;
  }

  template<typename Value_>
  void bindVariantValue(const std::string name, const int type_id, const Value_& value)
  {
	  auto array = Json::Value(Json::arrayValue);
    array.append(type_id);
    array.append(value);
    jsonVal[name] = array;
  }

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

  template<typename Variant_, typename EntityBindFn>
  void bindVariant(const std::string name, const Variant_ variant, EntityBindFn&& entityBind)
  {
    const int type_id = variant.getTypeId();

    variant.match(
      [&](auto val) -> EnableIfEntity<decltype(hana::decltype_(val))>
      {
        entityBind(bindVariantEntity(name, type_id));
      },
      [&](auto val) -> EnableIfEmpty<decltype(hana::decltype_(val))>
      {
        jsonVal[name] = type_id;
      },
      [&](auto val)
      {
        bindVariantValue(name, type_id, val)
      });
  }

};

}//jsoncpp
}//binders
}//nbdl

#endif
