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

class write
{
  Json::Value& json_val;

  public:

  write(Json::Value& value) :
    json_val(value)
  {}

  template<class X>
  void bind_member(X&& x)
  {
    json_val = std::forward<X>(x);
  }

  template<typename T>
  void bind_entity_member(const char* name, const T& field)
  { 
    nbdl::bind(write(json_val[name]), field);
  }

  template<typename X>
  void bind_sequence(X&& x)
  {
    nbdl::bind(*this, std::forward<X>(x));
  }

  template<typename X1, typename X2, typename... Xs>
  void bind_sequence(X1&& x1, X2&& x2, Xs&&... xs)
  {
    hana::for_each(
      hana::make_tuple(
        std::forward<X1>(x1),
        std::forward<X2>(x2),
        std::forward<Xs>(xs)...),
      [&](auto&& x) {
        Json::Value el;
        nbdl::bind(write(el), std::forward<decltype(x)>(x));
        json_val.append(el);
      });
  }


  template<typename V>
  void bind_variant(const V& variant)
  {
    const int type_id = variant.get_type_id();

    variant.match(
      [&](auto val) -> enable_if_empty<decltype(val)>
      {
        bind_sequence(type_id);
      },
      [&](auto val)
      {
        bind_sequence(type_id, val);
      });
  }

};

}//jsoncpp
}//binders

template<typename T>
struct bind_impl<binders::jsoncpp::write, T, enable_if_entity<T>>
{
  template<typename Binder, typename E>
  static void apply(Binder&& binder, E&& entity)
  {
    //todo need to actually bind to json object
    hana::for_each(entity_members<E>(),
      [&](auto&& member_type) {
        binder.bind_entity_member(member_name_(member_type), entity_member(entity, member_type));
      });
  }
};

}//nbdl

#endif
