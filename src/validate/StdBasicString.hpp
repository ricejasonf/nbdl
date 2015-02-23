#ifndef NBDL_VALIDATE_STDBASICSTRING_HPP
#define NBDL_VALIDATE_STDBASICSTRING_HPP

#include<string>
//#include<boost/algorithm/string/trim.hpp>
#include "Regex.hpp"

namespace nbdl {
namespace validation {
namespace {
	template<class T, class Traits, class Alloc>
	bool validateStdBasicStringLength(unsigned min, unsigned max, 
		std::basic_string<T, Traits, Alloc> &str)
	{
		return (str.size() > max || str.size() < min);
	}
}//anon

template<class M, class T, class Traits, class Alloc, class AddError>
struct ValidateString<M, std::basic_string<T, Traits, Alloc>, AddError>
{
	//todo mitigate template bloat as this is created for every member that is a string
	static void call(std::basic_string<T, Traits, Alloc> &member, AddError addError)
	{
		/* emscripten build unable to find boost for include path
		if (!MemberRawBuffer<M>::value)
			boost::trim(member);
		*/
		if (!MemberAllowBlank<M>::value && !member.size())
			return addError(ErrorToken::Required);
		if (validateStdBasicStringLength(
					MemberStringMinLength<M>::value,
					MemberStringMaxLength<M>::value,
					member))
			return addError(ErrorToken::OutOfRange);
		if (MemberMatch<M>::value != nullptr 
			&& !Regex::match(MemberMatch<M>::value, member))
			return addError(ErrorToken::MatchFail);
			
	}
};

}//validation
}//nbdl
#endif
