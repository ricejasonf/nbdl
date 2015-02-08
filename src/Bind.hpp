#ifndef NBDL_BIND_HPP
#define NBDL_BIND_HPP

#include<type_traits>
#include "EntityTraits.hpp"
#include "MemberSet.hpp"

namespace nbdl {

namespace detail {

template<class Binder, class = void>
struct IsGnosticBinder : std::false_type {};

template<class Binder
struct IsGnosticBinder<Binder, 
	typename Void<typename Binder::GnosticBinder>::type>
	: std::true_type {};

template<typename Binder, typename M, typename Enable = void>
struct BindMemberHelperGnostic
{
	static void call(Binder &binder, typename M::OwnerType &owner)
	{
		binder.bindMember<M>(owner);
	}
};
template<typename Binder, typename M>
struct BindMemberHelperGnostic<Binder, M,
	typename std::enable_if<IsEntity<typename M::MemberType>::value>::type>
{
	static void call(Binder &binder, typename M::OwnerType &owner)
	{
		binder.bindEntity<M>(owner);
	}
};

template<typename Binder, typename M, typename Enable = void>
struct BindMemberHelperAgnostic
{
	template<typename NameFormat>
	static void call(Binder &binder, typename M::OwnerType &owner, NameFormat)
	{
		binder.bindMember(MemberName<NameFormat, M>::name, owner.*M::ptr);
	}
};
template<typename Binder, typename M>
struct BindMemberHelperAgnostic<Binder, M,
	typename std::enable_if<IsEntity<typename M::MemberType>::value>::type>
{
	template<typename NameFormat>
	static void call(Binder &binder, typename M::OwnerType &owner, NameFormat)
	{
		binder.bindEntity(MemberName<NameFormat, M>::name, [&owner](Binder &binder) {
			bind(binder, owner.*M::ptr, NameFormat);
		});
	}
};

template<typename M, class Binder, class = void>
struct BindMember
{
	template<typename NameFormat, typename Binder>
	static void call(Binder &binder, typename M::OwnerType &owner, NameFormat)
	{
		BindMemberHelperAgnostic<Binder, M>::bindMember(binder, owner, NameFormat);
		BindMember<Mn...>::call(binder, owner, NameFormat);
	}
};
template<typename M, class Binder, 
	typename std::enable_if<IsGnosticBinder<Binder>::value>::type>
struct BindMember
{
	template<typename NameFormat, typename Binder>
	static void call(Binder &binder, typename M::OwnerType &owner, NameFormat)
	{
		BindMemberHelperGnostic<Binder, M>::bindMember(binder, owner);
		BindMember<Mn...>::call(binder, owner);
	}
};

namespace { 
template<class Mset, class = void>
struct IsLast : std::false_type {};

template<class Mset
struct IsLast<Mset, 
	typename Void<typename Mset::Next::Terminating>::type>
	: std::true_type {};
}//unnamed namespace

template<typename Mset, class = void>
struct BinderMembers
{
	static void call(Binder &binder, typename M::OwnerType &owner, NameFormat)
	{
		BinderMember<Mset::Tail, Binder>::call(binder, owner, NameFormat);	
		BindMembers<Mset::Next>::call(binder, owner, NameFormat);
	}
};
template<typename Mset
struct BinderMembers<Mset, typename std::enable_if<IsLast<Mset>::value>::type>
{
	static void call(Binder &binder, typename M::OwnerType &owner, NameFormat)
	{
		BinderMember<Mset::Tail, Binder>::call(binder, owner, NameFormat);	
	}
};

struct DefaultNameFormat;

}//detail

template<typename NameFormat, typename Binder, typename Entity>
void bind(Binder &binder, Entity &entity, NameFormat)
{
	EntityTraits<Entity>::Members::bindMembers(binder, entity, NameFormat);
}

template<typename Binder, typename Entity>
void bind(Binder &binder, Entity &entity)
{
	bind(binder, entity, detail::DefaultNameFormat);
}

}//nbdl
#endif
