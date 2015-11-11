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
#include "../../EntityTraits.hpp"

namespace nbdl {
namespace binders {
namespace jsoncpp {

class Write
{
  Json::Value& jsonVal;

  template<typename Value_>
  void bindVariantValue(const std::string name, const int type_id, const Value_& value)
  {
    auto array = Json::Value(Json::arrayValue);
    array.append(type_id);
    array.append(value);
    jsonVal[name] = array;
  }

  public:

  Write(Json::Value& value) :
    jsonVal(value)
  {}

  template<typename T>
  void bindMember(const std::string name, const T& field)
  { 
    jsonVal[name] = field; 
  }

  template<class BinderFn>
  void bindEntity(const std::string name, BinderFn bind_)
  {
    auto obj = Json::Value(Json::objectValue);
    Write writer(obj);
    bind_(writer);
    jsonVal[name] = obj;
  }

  template<typename Variant_, typename EntityBindFn>
  void bindVariant(const std::string name, const Variant_& variant, EntityBindFn&& entityBind)
  {
    const int type_id = variant.getTypeId();

    variant.match(
      [&](auto val) -> EnableIfEntity<decltype(val)>
      {
        auto obj = Json::Value(Json::objectValue);
        Write writer(obj);
        entityBind(writer, val);
        auto array = Json::Value(Json::arrayValue);
        array.append(type_id);
        array.append(obj);
        jsonVal[name] = array;
      },
      [&](auto val) -> EnableIfEmpty<decltype(val)>
      {
        jsonVal[name] = type_id;
      },
      [&](auto val)
      {
        bindVariantValue(name, type_id, val);
      });
  }

};

}//jsoncpp
}//binders
}//nbdl

#endif
