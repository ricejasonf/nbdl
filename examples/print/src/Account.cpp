#include "Account.h"

void Account::validate(EntityErrors &e)
{
	validateString(e, &nameFirst)
		.required()
		.max(50);
	validateString(e, "nameLast")
		.required()
		.max(50);
	validateString(e, "phoneNumber")
		.optional()
		.matches("^[0-9]{10}$");
}

void Account::bindMembers(Binder &b)
{
	bind(b, "nameFirst", nameFirst);
	bind(b, "nameLast", nameLast);
	bind(b, "phoneNumber", phoneNumber);
	bind(b, "address", address);
	bind(b, "foods", foods);
}

RelationMap Account::getRelationMap()
{
	static RelationMap map("account")
		.requireOwner(RelationMap::OneToOne("client"))
		.addMember(RelationMap::OneToOne("address"))
		.addMember(RelationMap::OneToMany("address"))
		.addMember(RelationMap::ManyToMany("contact"))
		;
	return map;
}
