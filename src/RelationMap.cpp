#include "RelationMap.h"

std::string RelationMap::getDefaultKeyName()
{
	return "id";
}

std::string RelationMap::getDefaultForeignKeyName(const std::string tableName)
{
	return tableName + "Id";
}

std::string RelationMap::getDefaultLinkingTableName(
	const std::string tableName1, 
	const std::string tableName2)
{
	return tableName1 + "_" + tableName2;
}

void RelationMap::normalize(OneToOne r)
{
	if (!r.key.size())
		r.key = getDefaultForeignKeyName(r.name);
	if (!r.foreignKey.size())
		r.foreignKey = getDefaultKeyName();
}

void RelationMap::normalize(OneToMany r)
{
	if (!r.key.size())
		r.key = getDefaultKeyName();
	if (!r.foreignKey.size())
		r.foreignKey = getDefaultForeignKeyName(name);
}

void RelationMap::normalize(ManyToMany r)
{
	if (!r.key.size())
		r.key = getDefaultKeyName();
	if (!r.foreignKey.size())
		r.foreignKey = getDefaultForeignKeyName(name);
	if (!r.linkingTable.size())
		r.linkingTable = getDefaultLinkingTableName(name, r.name);
}
