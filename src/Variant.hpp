#ifndef NBDL_VARIANT_HPP
#define NBDL_VARIANT_HPP

#include<type_traits>
#include "LambdaTraits.hpp"
#include "VariantCallback.hpp"

namespace nbdl {

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
struct VariantTypeSet {};
template<typename T1, typename... Tn>
struct VariantTypeSet<T1, Tn...>
{
	using Type = T1;
	using Next = VariantTypeSet<Tn...>;
	static const bool is_last = sizeof...(Tn) == 0;
	static const std::size_t type_id = sizeof...(Tn);
};

template<typename VtSet, typename FnSet, class = void>
struct VariantMatcher
{
	using Type = typename VtSet::Type;
	using Next = VariantMatcher<typename VtSet::Next, FnSet>;
	template<typename... Fns>
	static typename LambdaTraits<typename FnSet::Fn>::ReturnType call(std::size_t value_type_id, void *value, Fns... fns)
	{
		if (value_type_id == VtSet::type_id)
			return VariantCallback::call(*reinterpret_cast<Type*>(value), fns...);
		else
			return Next::call(value_type_id, value, fns...);
	}
};
template<typename VtSet, typename FnSet>
struct VariantMatcher<VtSet, FnSet,
	typename std::enable_if<VtSet::is_last>::type>
{
	using Type = typename VtSet::Type;
	template<typename... Fns>
	static typename LambdaTraits<typename FnSet::Fn>::ReturnType call(std::size_t value_type_id, void *value, Fns... fns)
	{
		//any invalid type_id (if it were possible) would result in the default type
		return VariantCallback::call(*reinterpret_cast<Type*>(value), fns...);
	}
};

template<typename... Tn>
struct VariantHelper
{
	static void copy(std::size_t src_type_id, void* src, void* dest) {}
	static void move(std::size_t src_type_id, void* src, void* dest) {}
	static void destroy(std::size_t type_id, void* value) {}
	template<typename Type>
	static std::size_t getTypeId()
	{
		return 0;
	}
	template<typename Fn1, typename... Fns>
	static typename LambdaTraits<Fn1>::ReturnType match(std::size_t value_type_id, void *value, Fn1 fn, Fns... fns)
	{
		return VariantCallback::call(value, fn, fns...);
	}
};
template<typename T, typename... Tn>
struct VariantHelper<T, Tn...>
{
	using Next = VariantHelper<Tn...>;
	static const std::size_t type_id = VariantTypeSet<T, Tn...>::type_id;

	static void copy(std::size_t src_type_id, void* src, void* dest)
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

	template<typename Fn1, typename... Fns>
	static typename LambdaTraits<Fn1>::ReturnType match(std::size_t value_type_id, void *value, Fn1 fn, Fns... fns)
	{
		if (value_type_id == type_id)
			return VariantCallback::call(value, fn, fns...);
		else
			return Next::match(value_type_id, value, fn, fns...);
	}
	
};

template<typename DefaultType, typename... Tn>
class Variant
{
	static_assert(std::is_empty<DefaultType>::value, "DefaultType must be an empty tag struct");
	//todo switch to use std::aligned_union once gcc 5.0 is available
	using Storage = typename std::aligned_storage<sizeof(details::MaxSizeOf<Tn...>::value)>::type;
	using Helper = VariantHelper<Tn..., DefaultType>;
	using VtSet = VariantTypeSet<Tn..., DefaultType>;
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
		type_id = 0; //in case shit goes horribly wrong
		Helper::destroy(type_id, &value_);
		new (&value_) Type(val);
		type_id = Helper::template getTypeId<Type>();
	}

	template<typename Fn1, typename... Fns>
	typename LambdaTraits<Fn1>::ReturnType match(Fn1 fn, Fns... fns)
	{
		using FnSet = VariantFnSet<Fn1, Fns...>;
		VariantMatcher<VtSet, FnSet>::call(type_id, &value_, fn, fns...);
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
