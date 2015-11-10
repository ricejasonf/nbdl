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
  using LastTypeId = hana::int_<hana::length(types()) - hana::size_c<1>>;

  int type_id;
  Storage value_;

  template<typename T>
  constexpr int typeIdFromType(T type)
  {
    return *hana::find(typeIds(), type);
  }

  template<typename Fn>
  void callByType(const int value_type_id, Fn&& fn)
  {
    hana::for_each(
      hana::range_c<int, 0, hana::length(types())>,
      [&](auto i) {
        if (value_type_id == hana::value(i))
          fn(hana::at(types(), i));
      });
  }
  
  void copy(const int src_type_id, const void* src, void* dest)
  {
    callByType(src_type_id, [&](auto matched) {
      using T = typename decltype(matched)::type;
      new (dest) T(*reinterpret_cast<const T*>(src));
    });
  }
  void move(const int src_type_id, const void* src, void* dest)
  {
    callByType(src_type_id, [&](auto matched) {
      using T = typename decltype(matched)::type;
      new (dest) T(std::move(*reinterpret_cast<const T*>(src)));
    });
  }
  void destroy(const int value_type_id, void* value)
  {
    callByType(value_type_id, [&](auto matched) {
      using T = typename decltype(matched)::type;
      reinterpret_cast<const T*>(value)->~T();
    });
  }

  template<typename Index, typename Fn, typename ReturnType>
  typename ReturnType::type matchByTypeHelper(Index i, const int type_id_x, Fn fn, ReturnType return_type)
  {
    if (type_id_x == Index::value)
      return fn(hana::at(types(), i));
    else
      return matchByTypeHelper(i + hana::int_c<1>, type_id_x, fn, return_type);
  }
  template<typename Fn, typename ReturnType>
  typename ReturnType::type matchByTypeHelper(LastTypeId i, const int type_id_x, Fn fn, ReturnType)
  {
    //if type_id_x is invalid, use default, empty type
    if (type_id_x == LastTypeId::value)
      return fn(hana::at(types(), i));
    else
      return fn(hana::at(types(), hana::int_c<0>));
  }

  public:

  //used to easily identify variant with sfinae
  using VariantTag = void;

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

  //for serialization
  int getTypeId()
  {
    return type_id;
  }

  //calls overload function with type type
  //used for serialization
  template<typename Fn1, typename... Fns>
  auto matchByType(const int type_id_x, Fn1 fn1, Fns... fns)
    -> decltype(hana::overload_linearly(fn1, fns...))
  {
    constexpr auto return_type = hana::type_c<decltype(hana::overload_linearly(fn1, fns...))>;

    //if type_id_x is invalid it will call with the default, empty type
    return matchByTypeHelper(
        hana::int_c<0>, 
        type_id_x, 
        hana::overload_linearly(fn1, fns...), 
        return_type);
  }

  bool isValidTypeId(int type_id_x)
  {
    return matchByType(type_id_x,
      [&](auto type) {
        return (type_id_x != 0 && typeIdFromType(type) == 0);
      });
  }

  //todo move to private
  template<typename ReturnType, typename... Fns>
  auto matchOverload(ReturnType, Fns... fns)
  {
    return hana::overload_linearly(fns...);
  }

  //todo move to private
  //for return types of void
  //don't require a match by
  //appending a no-op lambda 
  template<typename... Fns>
  auto matchOverload(hana::type<void>, Fns... fns)
  {
    return hana::overload_linearly(fns..., [](auto){});
  }

  //calls overload function with value
  //and allows no match when overloads return void
  template<typename Fn1, typename... Fns>
  typename LambdaTraits<Fn1>::ReturnType match(Fn1 fn1, Fns... fns)
  {
    static constexpr auto return_type = hana::type_c<typename LambdaTraits<Fn1>::ReturnType>;

    auto overload_ = matchOverload(return_type, fn1, fns...);

    return matchByTypeHelper(hana::int_c<0>, type_id, [&](auto type) {
        using T = typename decltype(type)::type;
        return overload_(*reinterpret_cast<T*>(&value_));
      }, return_type);
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
