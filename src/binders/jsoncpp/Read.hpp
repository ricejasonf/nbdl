//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDERS_JSONCPP_READ_HPP
#define NBDL_BINDERS_JSONCPP_READ_HPP

#include<string>
#include<jsoncpp/json/json.h>

namespace nbdl {
namespace binders {
namespace jsoncpp {

class Read
{
  void bind_(const Json::Value&, bool &);
  void bind_(const Json::Value&, unsigned int &);
  void bind_(const Json::Value&, int &);
  void bind_(const Json::Value&, double &);
  void bind_(const Json::Value&, std::string &);

  Read createObjectReader(const Json::Value&);

  template<typename Variant_>
  int getTypeId(const Json::Value& obj, Variant_ variant)
  {
    int type_id;
    if (obj.isIntegral())
    {
      type_id = obj.asInt();
      if (!variant.isEmptyType(type_id);
        throw std::runtime_error("Serialized variant is misrepresented as an empty type.");
    }
    else if (obj.isArray() && obj.size() == 2)
    {
      if (!obj[0u].isIntegral())
        throw std::runtime_error("Variant type_id is not integer.");
      type_id = obj[0u];
    }
    else
    {
      throw std::runtime_error("JSON representation of variant type expected");
    }
    if (!variant.isValidTypeId(type_id))
      throw std::runttime_error("Invalid type_id specified for variant.");
    return type_id;
  }

  const Json::Value &json_val;

  public:

  template<typename T>
  void bindMember(const std::string name, T &field)
  {
    bind_(json_val[name], field);
  }

  template<class BinderFn>
  void bindEntity(const std::string name, BinderFn&& bindFn)
  {
    Read reader = createObjectReader(json_val[name]);
    bindFn(reader);
  }

  //fn(int type_id,
  //  onTypeIsEntity()
  //  onTypeIsMember());
  template<typename Variant_, typename EntityBindFn>
  void bindVariant(const std::string name, Variant_ variant, EntityBindFn&& entityBind)
  {
    const Json::Value &obj = json_val[name];
    int type_id = getTypeId(obj, variant);

    variant.matchByType(type_id,
      [&](auto type) -> EnableIfEntity<decltype(type)>
      {
        Read reader = createObjectReader(obj[1u]); 
        entityBind(reader, type);
      },
      [&](auto type) -> EnableIfEmpty<decltype(type)>
      {
        using T =  typename decltype(type)::type;
        variant = T{};
      },
      [&](auto type)
      {
        using T =  typename decltype(type)::type;
        T value;
        bind_(obj[1u], value);
        variant = value;
      });
  }

  Read(const Json::Value &value);

  template<class EntityType>
  static void fromString(std::string &json, EntityType &entity)
  {
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(json, root, false))
    {
      throw std::runtime_error("JSON parse error");
    }
    Read r(root);
    entity.bindMembers(r);
  }
      
};

}//jsoncpp
}//binders
}//nbdl

#endif
