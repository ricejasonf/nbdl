//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_VARIANT_HPP
#define NBDL_VARIANT_HPP

#include<type_traits>
#include "boost/hana.hpp"
#include "LambdaTraits.hpp"

namespace nbdl {

using hana = boost::hana;

namespace details {

template<typename DefaultType, typename... Tn>
class Variant
{
  static_assert(std::is_empty<DefaultType>::value, "DefaultType must be an empty tag struct");
  using Storage typename std::aligned_union(sizeof(DefaultType), DefaultType, Tn...);
  using Helper = VariantHelper<Tn..., DefaultType>;
  static constexpr auto types = hana::tuple_t<DefaultType, Tn...>;
  std::size_t type_id;
  Storage value_;
  
  void copy(std::size_t src_type_id, const void* src, void* dest)
  {
    auto matched = hana::from_maybe(hana::type_c<DefaultType>, hana::find_if(types, hana::equal.to(type_id)));
    using T = typename decltype(matched)::type;
    new (dest) T(*reinterpret_cast<const T*>(src));
  }
  void move(std::size_t src_type_id, const void* src, void* dest)
  {
    auto matched = hana::from_maybe(hana::type_c<DefaultType>, hana::find_if(types, hana::equal.to(type_id)));
    using T = typename decltype(matched)::type;
    new (dest) T(std::move(*reinterpret_cast<const T*>(src)));
  }
  void destroy(std::size_t value_type_id, void* value)
  {
    auto matched = hana::from_maybe(hana::type_c<DefaultType>, hana::find_if(types, hana::equal.to(type_id)));
    using T = typename decltype(matched)::type;
    new (dest) T(*reinterpret_cast<const T*>(src));
    reinterpret_cast<const T*>(value)->~T();
  }

  public:

  Variant() : type_id(0) {}
  Variant(const Variant& old)
    : type_id(old.type_id)
  {
    copy(old.type_id, &old.value_, &value_);
  }
  Variant(Variant&& old)
    : type_id(old.type_id)
  {
    Helper::move(old.type_id, &old.value_, &value_);
  }
  ~Variant()
  {
    Helper::destroy(type_id, &value_);
  }
  Variant& operator= (Variant src)
  {
    std::swap(type_id, src.type_id);
    std::swap(value_, src.value_);
    return *this;
  }

  template<typename Type>
  Variant(Type val)
  {
    //it is critical that types are restricted to types supported by the Variant
    static_assert(hana::contains(types, hana::type_c<Type>));
    type_id = 0; //in case shit goes horribly wrong
    Helper::destroy(type_id, &value_);
    new (&value_) Type(val);
    type_id = Helper::template getTypeId<Type>();
  }

  template<typename Fn1, typename... Fns>
  LambdaTraits<Fn1>::ReturnType match(Fn1 fn1, Fns... fns)
  {
    static constexpr auto callbacks = hana::fold_left(hana::tuple_t<Fn1, Fns...>, hana::make_map(),
      [](auto map_, auto fn_) {
        auto fn = hana::decltype_(fn_);
        auto arg = hana::type_c<typename LambdaTraits<decltype(fn_)>::template Arg<0>>;
        return hana::insert(map_, hana::make_pair(hana::type_c<Arg>, Fn);
      });
    auto matched = hana::from_maybe(hana::type_c<DefaultType>, hana::find_if(types, hana::equal.to(type_id)));
    return callbacks[matched](*reinterpret_cast<typename decltype(matched)::type*>(&value_));
  }

};

}//details

//tags for empty variant value (always has type_id of 0)
struct Unresolved {};
struct Null {};

template<typename... Tn>
using Variant = details::Variant<Unresolved, Tn...>;

template<typename T>
using Optional = details::Variant<Null, T>;

}//nbdl

#endif
