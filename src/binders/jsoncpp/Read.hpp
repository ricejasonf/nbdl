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

class read
{
  void bind_(const Json::Value&, bool &);
  void bind_(const Json::Value&, unsigned int &);
  void bind_(const Json::Value&, int &);
  void bind_(const Json::Value&, double &);
  void bind_(const Json::Value&, std::string &);

  template<typename Variant_>
  int get_variant_type_id(const Variant_& variant)
  {
    int type_id;
    if (json_val.isIntegral())
    {
      type_id = json_val.asInt();
      variant.match_by_type(type_id,
        [&](auto type) -> enable_if_empty<decltype(type)> {},
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
    if (!variant.is_valid_type_id(type_id))
      throw std::runtime_error("Invalid type_id specified for variant.");
    return type_id;
  }

  const Json::Value &json_val;

  public:

  read(const Json::Value &value) :
    json_val(value)
  { }

  template<typename T>
  void bind_member(T&& field)
  {
    bind_(json_val, field);
  }

  template<typename T>
  void bind_entity_member(const char* name, T&& field)
  {
    nbdl::bind(read(json_val[name]), std::forward<T>(field));
  }

  template<typename X>
  void bind_sequence(X&& x)
  {
    nbdl::bind(*this, std::forward<X>(x));
  }
  
  template<typename X1, typename X2, typename... Xs>
  void bind_sequence(X1&& x1, X2&& x2, Xs&&... xs)
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
        nbdl::bind(read(*iter++), std::forward<decltype(x)>(x));
      });
  }

  template<typename V>
  void bind_variant(V&& variant)
  {
    int type_id = get_variant_type_id(variant);

    variant.match_by_type(type_id,
      [&](auto type) -> enable_if_empty<typename decltype(type)::type>
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
        nbdl::bind(read(contents), value);
        variant = value;
      });
  }

};

}//jsoncpp
}//binders

template<typename E_>
struct bind_impl<binders::jsoncpp::read, E_, enable_if_entity<E_>>
{
  template<typename Binder, typename E>
  static void apply(Binder&& binder, E&& entity)
  {
    hana::for_each(entity_members<E>(),
      [&](auto member_type) {
        binder.bind_entity_member(member_name_(member_type), entity_member(entity, member_type));
      });
  }
};

}//nbdl

#endif
