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

  const Json::Value &json_val;

  public:

  template<typename T>
  void bindMember(const std::string name, T &field)
  {
    bind_(json_val[name], field);
  }

  template<class BinderFn>
  void bindEntity(const std::string name, BinderFn bindFn)
  {
    Read reader = createObjectReader(json_val[name]);
    bindFn(reader);
  }

  //fn(int type_id,
  //  onTypeIsEntity()
  //  onTypeIsMember());
  template<typename T, Fn>
  void bindVariant(const std::string name, Fn fn)
  {
    const Json::Value &obj = json_val[name];
    if (obj.isIntegral())
    {
      auto oh_shit = [](const auto&) {
          throw std::runtime_error("Unserialized variant was supposed to be an empty type.");
        };
      fn(obj.asInt(), oh_shit, oh_shit);
    }
    else if (obj.isArray() && obj.size() == 2)
    {
      fn(obj[0u],
        [&](auto bindEntityFn) {
          Read reader = createObjectReader(obj[1u]); 
          bindEntityFn(reader);
        },
        [&](auto& value) {
          bind_(obj[1u], value);
        });
    }
    else
      throw std::runtime_error("JSON representation of variant type expected");
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
