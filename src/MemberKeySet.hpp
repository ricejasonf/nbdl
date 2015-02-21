#ifndef NBLD_MEMBERKEYSET
#define NBLD_MEMBERKEYSET

#include "MemberSet.hpp"

//don't even know if i need this yet
namespace nbdl {

template<typename... Mn>
struct MemberKeySet
{
	template<typename NameFormat, typename Binder, typename OwnerType>
	static void bindMembers(Binder &binder, OwnerType &owner)
	{}
};
template<typename M1, typename... Mn>
struct MemberKeySet<M1, Mn...>
{
	using Member = M1;
	using Next = MemberKeySet<Mn...>;
	using Offsets = detail::NumberSet<M1::offset, Mn::offset...>;

	template<typename M>
	static constexpr int indexOf()
	{
		return Offsets::template indexOf<M::offset>();
	}
};

template<class Mset, class = void>
struct MemberKeySetIsLast : std::true_type {};

template<class Mset>
struct MemberKeySetIsLast<Mset, typename Void<typename Mset::Next>::type>
	: std::false_type {};



}//nbdl

#endif
