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
#include "../../EntityTraits.hpp"

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
  int getTypeId(const Json::Value& obj, const Variant_& variant)
  {
    int type_id;
    if (obj.isIntegral())
    {
      type_id = obj.asInt();
      variant.matchByType(type_id,
        [&](auto type) -> EnableIfEmpty<decltype(type)> {},
        [&](auto)
        {
          throw std::runtime_error("Serialized variant is misrepresented as an empty type.");
        });
    }
    else if (obj.isArray() && obj.size() == 2)
    {
      if (!obj[0u].isIntegral())
        throw std::runtime_error("Variant type_id is not integer.");
      type_id = obj[0u].asInt();
    }
    else
    {
      throw std::runtime_error("JSON representation of variant type expected");
    }
    if (!variant.isValidTypeId(type_id))
      throw std::runtime_error("Invalid type_id specified for variant.");
    return type_id;
  }

  const Json::Value &json_val;

  public:

  Read(const Json::Value &value);

  template<typename T>
  void bindMember(T& field)
  {
    bind_(json_val, field);
  }

  template<typename T>
  void bindEntityMember(const char* name, T& field)
  {
    bind_(json_val[name], field);
  }


  template<typename... Xs>
  void bindSequence(Xs&&... xs)
  {
    if (!json_val.isArray())
      throw std::runtime_error("JSON Array expected when binding sequence.");
    auto iter = json_val.begin();
    auto f = [&](auto&& x) {
      nbdl::bind(Read(*iter++), std::forward<decltype(x)>(x));
    };
    f(xs)...;
  }

  //fn(int type_id,
  //  onTypeIsEntity()
  //  onTypeIsMember());
  template<typename Variant_, typename EntityBindFn>
  void bindVariant(const std::string name, Variant_& variant, EntityBindFn&& entityBind)
  {
    const Json::Value &obj = json_val[name];
    int type_id = getTypeId(obj, variant);

    variant.matchByType(type_id,
      [&](auto type) -> EnableIfEntity<typename decltype(type)::type>
      {
        Read reader = createObjectReader(obj[1u]); 
        entityBind(reader, type);
      },
      [&](auto type) -> EnableIfEmpty<typename decltype(type)::type>
      {
        using T = typename decltype(type)::type;
        variant = T{};
      },
      [&](auto type)
      {
        using T = typename decltype(type)::type;
        T value;
        bind_(obj[1u], value);
        variant = value;
      });
  }

};

}//jsoncpp
}//binders

template<typename E_>
struct BindImpl<binders::jsoncpp::Read, E_, EnableIfEntity<E_>>
{
  template<typename Binder, typename E>
  void apply(Binder&& binder, E&& entity)
  {
    hana::foreach(EntityTraits<E_>::Members{},
      [&](auto&& member_type) {
        using Member_ = typename decltype(member_type)::type;
        binder.bindEntityMember(MemberName<E_>::value, entity.*Member_::ptr);
      });
  }
};

}//nbdl

#endif
