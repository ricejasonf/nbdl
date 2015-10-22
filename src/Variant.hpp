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
  using Storage = std::aligned_union<sizeof(DefaultType), DefaultType, Tn...>;

  static constexpr auto types = hana::tuple_t<DefaultType, Tn...>;
  /*
  static constexpr auto types = hana::unpack(hana::zip_with(hana::make_pair,
    hana::unpack(hana::make_range(hana::int_c<0>, hana::int_c<(sizeof...(Tn) + 1)>), hana::make_tuple),
    hana::tuple_t<DefaultType, Tn...>
  ), hana::make_map);
  */
  template<typename Fn>
  void callByType(const std::size_t value_type_id, Fn&& fn)
  {
    hana::for_each(
      hana::range_c<int, 0, hana::length(types)>,
      [&](auto i) {
        if (value_type_id == hana::value(i))
          fn(hana::at(types, i));
      });
  }

  std::size_t type_id;
  Storage value_;
  
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
    static_assert(hana::contains(types, hana::type_c<Type>), "Variant does not support conversion to Type.");
    type_id = 0; //in case shit goes horribly wrong
    destroy(type_id, &value_);
    new (&value_) Type(val);
    type_id = hana::while_(
      [](auto i) {
        return (hana::at(types, i) != hana::type_c<Type>);
      },
      hana::int_c<0>,
      [](auto i) {
        return i + hana::int_c<1>;
      });
  }

  template<typename Fn1, typename... Fns>
  typename LambdaTraits<Fn1>::ReturnType match(Fn1&& fn1, Fns&&... fns)
  {
    static constexpr auto callbacks = hana::fold_left(hana::tuple_t<Fn1, Fns...>, hana::make_map(),
      [](auto map_, auto fn_) {
        auto arg = hana::type_c<typename LambdaTraits<decltype(fn_)>::template Arg<0>>;
        return hana::insert(map_, hana::make_pair(arg, hana::size_c<hana::length(map_)>));
      });
    auto matched = hana::from_maybe(hana::type_c<DefaultType>, hana::at(types, type_id));
    callByType(type_id, 
      [&](auto matched) {
         
      });
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
