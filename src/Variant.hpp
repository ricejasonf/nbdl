#ifndef NBDL_VARIANT_HPP
#define NBDL_VARIANT_HPP

#include<type_traits>

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

template<typename Tn...>
struct VariantHelper
{
	static void copy(std::size_t src_type_id, void* src, void* dest) {}
	static void move(std::size_t src_type_id, void* src, void* dest) {}
	static void destroy(std::size_t type_id, void* value) {}
	template<typename Type>
	std::size_t getTypeId()
	{
		return 0;
	}
}
template<typename T, typename... Tn>
struct VariantHelper<T, Tn...>
{
	using Next = VariantHelper<Tn...>;
	static const std::size_t type_id = sizeof...(Tn) + 1;

	template<typename Type>
	std::size_t getTypeId()
	{
		if (std::is_same<Type, T>::value)
			return type_id;
		else
			return Next::getTypeId<Type>();
	}

	static void copy(std::size_t src_type_id, void* src, void* dest)
	{
		if (src_type_id == type_id)
			new (dest) T(reinterpret_cast<const T*>(src));
		else
			Next::copy(src_type_id, src, dest);
	}

	static void move(std::size_t src_type_id, void* src, void* dest)
	{
		if (src_type_id == type_id)
			new (dest) T(std::move(reinterpret_cast<const T*>(src)));
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
	
};

template<typename DefaultType, typename... Tn>
class Variant
{
	static_assert(std::is_empty(DefaultType)::value, "DefaultType must be an empty tag struct");
	//todo switch to use std::aligned_union once gcc 5.0 is available
	using Storage = typename std::aligned_storage<sizeof(details::MaxSizeOf<Tn...>::value)>::type;
	using Helper = VariantHelper<Tn..., DefaultType>;
	static std::size_t type_id;
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
		Helper::destroy(type_id, &value_)
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
		new (&data) Type(val);
		type_id = Helper::getTypeId<Type>();
	}
};

}//details

//tags for empty variant value (always has type_id of 0)
struct Unresolved {};
struct Null {};

template<typename... Tn>
class Variant : public details::Variant<Unresolved, Tn...> {};

template<typename T>
class Optional : public details::Variant<Null, T> {};

}//nbdl

#endif
