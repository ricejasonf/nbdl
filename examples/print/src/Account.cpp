#include "Account.h"

void Account::validate()
{
	validateString("nameFirst")
		.required()
		.max(50);
	validateString("nameLast")
		.required()
		.max(50);
	validateString("phoneNumber")
		.optional()
		.matches("^[0-9]{10}$");
}

void Account::bindMembers(Binder &b)
{
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
