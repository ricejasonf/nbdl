//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_VARIANT_HPP
#define NBDL_VARIANT_HPP

#include<details/CommonType.hpp>

#include<type_traits>
#include<boost/hana.hpp>

namespace nbdl {

namespace hana = boost::hana;

namespace details {

template<typename DefaultType, typename... Tn>
class Variant
{
  static_assert(std::is_empty<DefaultType>::value, "DefaultType must be an empty tag struct");
  using Storage = typename std::aligned_union<sizeof(DefaultType), DefaultType, Tn...>::type;

  static constexpr auto types() { return hana::tuple_t<DefaultType, Tn...>; }
  static auto typeIds()
  {
    return hana::unpack(hana::range_c<int, 1, sizeof...(Tn)+1>, [](auto ...i) {
      return hana::make_map(
        hana::make_pair(hana::type_c<DefaultType>, hana::int_c<0>),
        hana::make_pair(hana::type_c<Tn>, i)...
      );
    });
  }
  using LastTypeId = hana::int_<sizeof...(Tn)>;

  int type_id;
  Storage value_;

  template<typename T>
  constexpr int typeIdFromType() const
  {
    return *hana::find(typeIds(), hana::type_c<T>);
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

  template<typename Index, typename Fn>
  auto matchByTypeHelper(Index i, const int type_id_x, Fn fn) const
    ->  nbdl::detail::common_type_t<
          decltype(fn(hana::at(types(), i))),
          decltype(fn(hana::at(types(), hana::int_c<0>)))
        >
  {
    if (type_id_x == Index::value)
      return fn(hana::at(types(), i));
    else
      return matchByTypeHelper(i + hana::int_c<1>, type_id_x, fn);
  }
  template<typename Fn>
  auto matchByTypeHelper(LastTypeId i, const int type_id_x, Fn fn) const
    ->  nbdl::detail::common_type_t<
          decltype(fn(hana::at(types(), i))),
          decltype(fn(hana::at(types(), hana::int_c<0>)))
        >
  {
    //if type_id_x is invalid, use default, empty type
    if (type_id_x == LastTypeId::value)
      return fn(hana::at(types(), i));
    else
      return fn(hana::at(types(), hana::int_c<0>));
  }

  template<typename Fn1>
  auto matchOverload(Fn1 fn1) const
  {
    return fn1;
  }
  template<typename Fn1, typename Fn2, typename... Fns>
  auto matchOverload(Fn1 fn1, Fn2 fn2, Fns... fns) const
  {
    return hana::overload_linearly(fn1, fn2, fns...);
  }

  //provides a better compiler error that outputs 
  //the type in question should it be invalid
  template<typename Type, typename TypeType>
  auto convertFromType(const Type& val, TypeType t)
    -> std::enable_if_t<hana::contains(types(), t)>
  {
    //it is critical that types are restricted to types supported by the Variant
    //this check is now redundant
    static_assert(hana::contains(types(), hana::type_c<Type>),
      "This variant does not support conversion to Type.");

    type_id = 0; //in case shit goes horribly wrong
    destroy(type_id, &value_);
    new (&value_) Type(val);
    type_id = typeIdFromType<Type>();
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
    convertFromType(val, hana::type_c<Type>);
  }

  //for serialization
  int getTypeId() const
  {
    return type_id;
  }

  bool isValidTypeId(int type_id_x) const
  {
    // perhaps this should just assume that type_ids are sequential indices
    if (type_id_x == 0)
    {
      return true;
    }
    else
    {
      return matchByType(type_id_x,
        [&](auto type) {
          return (type_id_x == typeIdFromType<typename decltype(type)::type>());
        });
    }
  }

  //calls overload function with type type
  //used for serialization
  template<typename... Fns>
  auto matchByType(const int type_id_x, Fns... fns) const
  {
    auto overload_ = matchOverload(fns...);

    //if type_id_x is invalid it will call with the default, empty type
    return matchByTypeHelper(
        hana::int_c<0>,
        type_id_x,
        overload_
      );
  }

  template<typename T>
  bool is()
  {
    return (type_id == typeIdFromType<T>());
  }

  template<typename... Fns>
  auto match(Fns... fns) const
  {
    auto overload_ = matchOverload(fns...);

    return matchByTypeHelper(hana::int_c<0>, type_id, [&](auto type) {
        using T = typename decltype(type)::type;
        return overload_(*reinterpret_cast<const T*>(&value_));
      });
  }

};

}//details

//useful for match catch all
auto noop = [](auto){};

//tags for empty variant value (always has type_id of 0)
struct Unresolved {};
struct Null {};

template<typename... Tn>
using Variant = details::Variant<Unresolved, Tn...>;

template<typename T>
using Optional = details::Variant<Null, T>;

}//nbdl

#endif
