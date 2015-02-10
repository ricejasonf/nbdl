#ifndef NBDL_VALIDATESTRING_HPP
#define NBDL_VALIDATESTRING_HPP

namespace nbdl {
namespace detail {

template<typename M, typename Errors>
bool validateStringMaxLength(Errors &e, M::MemberType &m)
{
	if (m.size() > MemberStringMaxLength<M>::value)
		e.addError(errors::TooLong{});
}

template<typename M, typename Errors, typename T>
void validateString(Errors &e, T& member) {}

template<typename M, typename Errors, class T, class Traits, class Alloc>
void validateString(Errors &e, basic_string<T, Traits, Alloc> &member)
{
	//todo trim whitespace
	if (!validateStringMaxLength<Errors, M>(errors, member))
		return;
}

}//detail
}//nbdl
#endif
