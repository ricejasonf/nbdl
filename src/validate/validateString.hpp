#ifndef NBDL_VALIDATE_STDBASICSTRING_HPP
#define NBDL_VALIDATE_STDBASICSTRING_HPP

namespace nbdl {
namespace validate {

template<typename M, typename Errors, class T, class Traits, class Alloc>
void validateString(Errors &e, std::basic_string<T, Traits, Alloc> &member)
{
	//todo trim whitespace
	if (m.size() > MemberStringMaxLength<M>::value)
	{
		e.addError(ErrorToken::TooLong);
		return;
	}
}

}//validate
}//nbdl
#endif
