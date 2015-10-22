//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_VARIANT_HPP
#define NBDL_VARIANT_HPP

#include<type_traits>
#include<boost/hana.hpp>
#include "LambdaTraits.hpp"
#include "VariantCallback.hpp"

namespace nbdl {

namespace hana = boost::hana;

namespace details {

template<typename DefaultType, typename... Tn>
class Variant
{
  static_assert(std::is_empty<DefaultType>::value, "DefaultType must be an empty tag struct");
  using Storage = typename std::aligned_union<sizeof(DefaultType), DefaultType, Tn...>::type;

  static constexpr auto types() { return hana::tuple_t<DefaultType, Tn...>; }
  static constexpr auto typeIds()
  { 
    return hana::unpack(hana::zip_with(hana::make_pair,
      hana::tuple_t<DefaultType, Tn...>,
      hana::unpack(hana::make_range(hana::int_c<0>, hana::int_c<(sizeof...(Tn) + 1)>), hana::make_tuple)
    ), hana::make_map);
  }
  using LastTypeId = hana::size_t<hana::length(types()) - hana::size_c<1>>;

  std::size_t type_id;
  Storage value_;

  template<typename T>
  constexpr std::size_t typeIdFromType(T type)
  {
    return *hana::find(typeIds(), type);
  }

  template<typename Fn>
  void callByType(const std::size_t value_type_id, Fn&& fn)
  {
    hana::for_each(
      hana::range_c<int, 0, hana::length(types())>,
      [&](auto i) {
        if (value_type_id == hana::value(i))
          fn(hana::at(types(), i));
      });
  }
  
  void copy(std::size_t src_type_id, const void* src, void* dest)
  {
    callByType(src_type_id, [&](auto matched) {
      using T = typename decltype(matched)::type;
      new (dest) T(*reinterpret_cast<const T*>(src));
    });
  }
  void move(std::size_t src_type_id, const void* src, void* dest)
  {
    callByType(src_type_id, [&](auto matched) {
      using T = typename decltype(matched)::type;
      new (dest) T(std::move(*reinterpret_cast<const T*>(src)));
    });
  }
  void destroy(std::size_t value_type_id, void* value)
  {
    callByType(value_type_id, [&](auto matched) {
      using T = typename decltype(matched)::type;
      reinterpret_cast<const T*>(value)->~T();
    });
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
    move(old.type_id, &old.value_, &value_);
  }
  ~Variant()
  {
    destroy(type_id, &value_);
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
    //this check is now redundant, but provides a better error message
    static_assert(hana::contains(types(), hana::type_c<Type>),
      "Variant does not support conversion to Type.");
    type_id = 0; //in case shit goes horribly wrong
    destroy(type_id, &value_);
    new (&value_) Type(val);
    type_id = typeIdFromType(hana::type_c<Type>);
  }

  template<typename Index, typename Fn1, typename... Fns>
  typename LambdaTraits<Fn1>::ReturnType matchHelper(Index i, Fn1 fn1, Fns... fns)
  {
    static constexpr auto current_type = hana::at(types(), i);
    using T = typename decltype(current_type)::type;
    if (type_id == Index::value)
      return VariantCallback::call(*reinterpret_cast<T*>(&value_), fn1, fns...);
    else
      return matchHelper(i + hana::size_c<1>, fn1, fns...);
  }
  template<typename Fn1, typename... Fns>
  typename LambdaTraits<Fn1>::ReturnType matchHelper(LastTypeId i, Fn1 fn1, Fns... fns)
  {
    static constexpr auto current_type = hana::at(types(), i);
    using T = typename decltype(current_type)::type;
    return VariantCallback::call(*reinterpret_cast<T*>(&value_), fn1, fns...);
  }
  template<typename Fn1, typename... Fns>
  typename LambdaTraits<Fn1>::ReturnType match(Fn1 fn1, Fns... fns)
  {
    return matchHelper(hana::size_c<0>, fn1, fns...);
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
