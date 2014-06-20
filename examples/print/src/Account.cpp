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

RelationMap Account::getRelationMap()
{
	static RelationMap map
		.table("account");
		.requireOwner(
			RelationMap::OneToOne("client")
				.withKey("clientId")
		).addMember(
			RelationMap::OneToOne("address")
				.withKey("addressId")
		).addMember(
			RelationMap::OneToMany("address")
					.withForeignKey("propertyId")
		).addMember(
			MemberManyToMany("contact")
				.withLinkingTable("accountContact")
				.withKey("contactId")
				.withForeignKey("accountId")
		);
	return map;
}
