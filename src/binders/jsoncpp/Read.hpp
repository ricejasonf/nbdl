//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDERS_JSONCPP_READ_HPP
#define NBDL_BINDERS_JSONCPP_READ_HPP

#include<tuple>
#include<boost/hana.hpp>
#include<functional> //std::ref
#include<string>
#include<jsoncpp/json/json.h>
#include "../../Traits.hpp"
#include "../../Bind.hpp"
#include "../../Member.hpp"

namespace nbdl {
namespace hana = boost::hana;
namespace binders {
namespace jsoncpp {

class Read
{
  void bind_(const Json::Value&, bool &);
  void bind_(const Json::Value&, unsigned int &);
  void bind_(const Json::Value&, int &);
  void bind_(const Json::Value&, double &);
  void bind_(const Json::Value&, std::string &);

  template<typename Variant_>
  int getVariantTypeId(const Variant_& variant)
  {
    int type_id;
    if (json_val.isIntegral())
    {
      type_id = json_val.asInt();
      variant.matchByType(type_id,
        [&](auto type) -> EnableIfEmpty<decltype(type)> {},
        [&](auto)
        {
          throw std::runtime_error("Serialized variant is misrepresented as an empty type.");
        });
    }
    else if (json_val.isArray() && json_val.size() == 2)
    {
      if (!json_val[0u].isIntegral())
        throw std::runtime_error("Variant type_id is not integer.");
      type_id = json_val[0u].asInt();
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

  Read(const Json::Value &value) :
    json_val(value)
  { }

  template<typename T>
  void bindMember(T&& field)
  {
    bind_(json_val, field);
  }

  template<typename T>
  void bindEntityMember(const char* name, T&& field)
  {
    nbdl::bind(Read(json_val[name]), std::forward<T>(field));
  }

  template<typename X>
  void bindSequence(X&& x)
  {
    nbdl::bind(*this, std::forward<X>(x));
  }
  
  template<typename X1, typename X2, typename... Xs>
  void bindSequence(X1&& x1, X2&& x2, Xs&&... xs)
  {
    if (!json_val.isArray() || json_val.size() != sizeof...(Xs) + 2)
      throw std::runtime_error("JSON Array expected when binding sequence.");
    auto iter = json_val.begin();
    hana::for_each(
      hana::make_tuple(
        std::ref(x1),
        std::ref(x2),
        std::ref(xs)...),
      [&](auto&& x) {
        nbdl::bind(Read(*iter++), std::forward<decltype(x)>(x));
      });
  }

  template<typename V>
  void bindVariant(V&& variant)
  {
    int type_id = getVariantTypeId(variant);

    variant.matchByType(type_id,
      [&](auto type) -> EnableIfEmpty<typename decltype(type)::type>
      {
        using T = typename decltype(type)::type;
        variant = T{};
      },
      [&](auto type)
      {
        using T = typename decltype(type)::type;
        T value;
        Json::Value contents = json_val[1u];
        if (contents.isNull())
          throw std::runtime_error("JSON value expected for contents of variant");
        nbdl::bind(Read(contents), value);
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
  static void apply(Binder&& binder, E&& entity)
  {
    hana::for_each(entityMembers<E>(),
      [&](auto member_type) {
        binder.bindEntityMember(memberName(member_type), entityMember(entity, member_type));
      });
  }
};

}//nbdl

#endif
