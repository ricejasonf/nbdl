#include "Account.h"
#include "RelationMap.h"

void Account::validate(ErrorBinder &e)
{
	validateString(e, nameFirst)
		.required()
		.maxLen(50);
	validateString(e, nameLast)
		.required()
		.maxLen(50);
	validateString(e, phoneNumber)
		.optional()
		.matches("^[0-9]{10}$");
}

void Account::bindMembers(Binder &b)
{
	bindReadOnly(b, "nameFirst", nameFirst);
	bindReadOnly(b, "nameLast", nameLast);
	bind(b, "phoneNumber", phoneNumber);
	bind(b, "age", age);
	bind(b, "address", address);
	bind(b, "foods", foods);
}

RelationMap Account::getRelationMap()
{
	static auto map = RelationMap("account")
		.requireOwner(RelationMap::OneToOne("client"))
		.addMember(RelationMap::OneToOne("address"))
		.addMember(RelationMap::OneToMany("address"))
		.addMember(RelationMap::ManyToMany("contact"))
		;
	return map;
}
