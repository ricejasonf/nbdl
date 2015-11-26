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
#include "../../Bind.hpp"
#include "../../Member.hpp"

namespace nbdl {
namespace binders {
namespace jsoncpp {

class Write
{
  Json::Value& json_val;

  public:

  Write(Json::Value& value) :
    json_val(value)
  {}

  template<class X>
  void bindMember(X&& x)
  {
    json_val = std::forward<X>(x);
  }

  template<typename T>
  void bindEntityMember(const char* name, const T& field)
  { 
    nbdl::bind(Write(json_val[name]), field);
  }

  template<typename X>
  void bindSequence(X&& x)
  {
    nbdl::bind(*this, std::forward<X>(x));
  }

  template<typename X1, typename X2, typename... Xs>
  void bindSequence(X1&& x1, X2&& x2, Xs&&... xs)
  {
    hana::for_each(
      hana::make_tuple(
        std::forward<X1>(x1),
        std::forward<X2>(x2),
        std::forward<Xs>(xs)...),
      [&](auto&& x) {
        Json::Value el;
        nbdl::bind(Write(el), std::forward<decltype(x)>(x));
        json_val.append(el);
      });
  }


  template<typename V>
  void bindVariant(const V& variant)
  {
    const int type_id = variant.getTypeId();

    variant.match(
      [&](auto val) -> EnableIfEmpty<decltype(val)>
      {
        bindSequence(type_id);
      },
      [&](auto val)
      {
        bindSequence(type_id, val);
      });
  }

};

}//jsoncpp
}//binders

template<typename T>
struct BindImpl<binders::jsoncpp::Write, T, EnableIfEntity<T>>
{
  template<typename Binder, typename E>
  static void apply(Binder&& binder, E&& entity)
  {
    //todo need to actually bind to json object
    hana::for_each(entityMembers<E>(),
      [&](auto&& member_type) {
        binder.bindEntityMember(memberName(member_type), entityMember(entity, member_type));
      });
  }
};

}//nbdl

#endif
