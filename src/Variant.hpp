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

template<typename... Vn>
struct VariantTypeSet {};

template<typename V1, typename... Vn>
struct VariantTypeSet<V1, Vn...>
{
	using Type = V1;	
	using Next = VariantTypeSet<Vn...>;
	static const unsigned id = sizeof...(Vn);
};

template<typename Set, unsigned id>
struct GetVariantType
{
	static void call(const unsigned id)
	{
		if (id == Set::id)
			return 
	}
};

}//details

template<typename... Tn>
class Variant
{
	using Data = typename std::aligned_storage<sizeof(details::MaxSizeOf<Tn...>::value)>::type;
	unsigned int type;
	Data value_;

	public:

	template<typename T>
	Variant(const T& other) : value_(other) {}
	template<typename T>
	Variant(const T&& other) : value_(other) {}
};

}//nbdl

#endif
