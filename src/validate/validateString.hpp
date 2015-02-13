#ifndef NBDL_VALIDATE_STDBASICSTRING_HPP
#define NBDL_VALIDATE_STDBASICSTRING_HPP

namespace nbdl {
namespace validation {

template<class M, class T, class Traits, class Alloc, class AddError>
void validateString<M, std::basic_string<T, Traits, Alloc>, AddErrors>
	(std::basic_string<T, Traits, Alloc> &member, AddError addError)
{
	//todo trim whitespace
	if (member.size() > MemberStringMaxLength<M>::value)
	{
		addError(ErrorToken::TooLong);
		return;
	}
}

}//validation
}//nbdl
#endif
