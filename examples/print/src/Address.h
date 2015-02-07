#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <EntityTraits.hpp>

struct Address
{
	std::string line1;
	std::string line2;
	std::string city;
	std::string state;
	std::string zipCode;
};
namespace nbdl {
	NBDL_ENTITY(
		Address,
			line1,
			line2,
			city,
			state,
			zipCode );
}//nbdl
#endif
