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
//#include "VariantCallback.hpp"

namespace nbdl {

using hana = boost::hana;

namespace details {

template<typename... Tn>
struct MaxSizeOf
{
	static const std::size_t value = sizeof(char);
};
template<typename T1, typename... Tn>
struct MaxSizeOf<T1, Tn...>
{
	static const std::size_t value = MaxSizeOf<Tn...>::value > sizeof(T1) ?
		MaxSizeOf<Tn...>::value : sizeof(T1);
};

template<typename... Tn>
struct VariantHelper
{
	static void copy(std::size_t, const void*, void*) {}
	static void move(std::size_t, void*, void*) {}
	static void destroy(std::size_t, void*) {}
	template<typename Type>
	static std::size_t getTypeId()
	{
		return 0;
	}
};
template<typename T, typename... Tn>
struct VariantHelper<T, Tn...>
{
	using Next = VariantHelper<Tn...>;
	static const std::size_t type_id = VariantTypeSet<T, Tn...>::type_id;

	static void copy(std::size_t src_type_id, const void* src, void* dest)
	{
		if (src_type_id == type_id)
			new (dest) T(*reinterpret_cast<const T*>(src));
		else
			Next::copy(src_type_id, src, dest);
	}

	static void move(std::size_t src_type_id, void* src, void* dest)
	{
		if (src_type_id == type_id)
			new (dest) T(std::move(*reinterpret_cast<T*>(src)));
		else
			Next::move(src_type_id, src, dest);
	}

	static void destroy(std::size_t value_type_id, void* value)
	{
		if (value_type_id == type_id)
			reinterpret_cast<const T*>(value)->~T();
		else
			Next::destroy(value_type_id, value);
	}

	template<typename Type>
	static std::size_t getTypeId()
	{
		if (std::is_same<Type, T>::value)
			return type_id;
		else
			return Next::template getTypeId<Type>();
	}
	
};

template<typename DefaultType, typename... Tn>
class Variant
{
	static_assert(std::is_empty<DefaultType>::value, "DefaultType must be an empty tag struct");
	//todo switch to use std::aligned_union once gcc 5.0 is available
	using Storage = typename std::aligned_storage<sizeof(details::MaxSizeOf<Tn...>::value)>::type;
	using Helper = VariantHelper<Tn..., DefaultType>;
  static constexpr auto types = hana::tuple_t<DefaultType, Tn...>;
	std::size_t type_id;
	Storage value_;

	public:

	Variant() : type_id(0) {}
	Variant(const Variant& old)
		: type_id(old.type_id)
	{
		Helper::copy(old.type_id, &old.value_, &value_);
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
    return callbacks[matched](*reinterpret_cast<decltype(matched)::type*>(&value_));
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
