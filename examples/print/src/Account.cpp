#include "Account.h"

void Account::validate()
{
	validateString("NameFirst")
		.required()
		.max(50);
	validateString("NameLast")
		.required()
		.max(50);
	validateString("PhoneNumber")
		.optional()
		.matches("^[0-9]{10}$");
}

Client Account::getClient()
{
	return getRelation<Client>("Client", client);	
}

Client Account::setClient(Client client)
{
	return setRelation<Client>(client);
}

void Account::bindMembers(Binder &b)
{
	bind(b, "address", &address);
}

	/*
RelationMap Account::getRelationMap()
{
	static RelationMap map
		.table("account");
		.add(
			MemberOneToOne("address")
				.withKey("addressId")
		).add(
			MemberOneToMany("address")
				.withForeignKey("propertyId")
		).add(
			MemberManyToMany("contact")
				.withLinkingTable("accountContact")
				.withKey("contactId")
				.withForeignKey("accountId")
		);
	return map;
}
*/
