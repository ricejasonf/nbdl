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

/*
Client Account::getClient()
{
	return getRelation<Client>("Client", client);	
}

Client Account::setClient(Client client)
{
	return setRelation<Client>(client);
}
*/

void Account::bindMembers(Binder &b)
{
	bind(b, "address", address);
	bind(b, "foods", foods);
}

	/*
RelationMap Account::getRelationMemberMap()
{
	static RelationMap map
		.table("account");
		.add(
			RelationMap::OneToOne("address")
				.withKey("addressId")
		).add(
		RelationMap::OneToMany("address")
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
