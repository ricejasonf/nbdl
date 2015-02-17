#ifndef NBDL_VALIDATE_STDBASICSTRING_HPP
#define NBDL_VALIDATE_STDBASICSTRING_HPP

namespace nbdl {
namespace validation {

template<class M, class T, class Traits, class Alloc, class AddError>
struct ValidateString<M, std::basic_string<T, Traits, Alloc>, AddError>
{
	//todo mitigate template bloat
	static void call(std::basic_string<T, Traits, Alloc> &member, AddError addError)
	{
		//todo trim whitespace
		if (member.size() > MemberStringMaxLength<M>::value)
		{
			addError(ErrorToken::TooLong);
			return;
		}
	}
};

}//validation
}//nbdl
#endif
