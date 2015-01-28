#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <EntityTraits.hpp>
//#include <Entity.hpp>

struct Address
{
	std::string line1;
	std::string line2;
	std::string city;
	std::string state;
	std::string zipCode;

	/*todo move to diff class
	template<class ValidationBinder>
	void validate(ValidationBinder &e)
	{
		validateString(e, line1)
			.required()
			.maxLen(50);
		validateString(e, line2)
			.optional()
			.maxLen(50);
		validateString(e, zipCode)
			.required()
			.matches("^[0-9]{5}$");
		validateString(e, city)
			.required();
		validateString(e, state)
			.required()
			.matches("^[A-Z]{2}$");
	}

	template<class Binder>
	void members(Binder &b)
	{
		member(b, "line1", line1);
		member(b, "line2", line2);
		member(b, "city", city);
		member(b, "state", state);
		member(b, "zipCode", zipCode);
	}
	*/
};
namespace nbdl {
template<>
struct EntityTraits<Address>
{
	using Members = MemberSet<
		NBDL_MEMBER(&Address::line1),
		NBDL_MEMBER(&Address::line2),
		NBDL_MEMBER(&Address::city),
		NBDL_MEMBER(&Address::state),
		NBDL_MEMBER(&Address::zipCode) >;
};
NBDL_MEMBER_NAME(Address, line1);
NBDL_MEMBER_NAME(Address, line2);
NBDL_MEMBER_NAME(Address, city);
NBDL_MEMBER_NAME(Address, state);
NBDL_MEMBER_NAME(Address, zipCode);
}//nbdl

#endif
