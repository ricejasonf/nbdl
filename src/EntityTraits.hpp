#ifndef NBDL_ENTITYTRAITS_HPP
#define NBDL_ENTITYTRAITS_HPP

namespace nbdl {

template<typename Impl>
struct EntityTraits;

template<typename T>
struct Void
{
	using type = void;
};
template<class T, class U = void>
struct IsEntity
{
	enum { value = 0 };
};
template<class T>
struct IsEntity<T, typename Void<typename EntityTraits<T>::Members>::type>
{
	enum { value = 1 };
};

template<class Set, class = void>
struct TypeSetIsLast : std::true_type {};

template<class Set>
struct TypeSetIsLast<Set, typename Void<typename Set::Next>::type>
	: std::false_type {};

}//nbdl

//todo remove these
#include "macros/NBDL_ENTITY.hpp"

#endif
