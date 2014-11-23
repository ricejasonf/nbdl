#ifndef RELATIONMAP_H
#define RELATIONMAP_H

#include<string>
#include<vector>

class RelationMap
{
	public:
	
	RelationMap() = delete;
	RelationMap(std::string name) : name(name) {}

	virtual ~RelationMap() {}

	struct OneToOne
	{
		std::string name;
		std::string key;
		std::string foreignKey;

		OneToOne(const std::string name) : name(name) {}
		OneToOne &withKey(std::string name) { key = name; return *this; }
		OneToOne &withForeignKey(std::string name) { foreignKey = name; return *this; } //default to 'id'
	};

	struct OneToMany
	{
		std::string name;
		std::string key;
		std::string foreignKey;

		OneToMany(const std::string name) : name(name) {}
		OneToMany &withKey(std::string name) { key = name; } //default to 'id'
		OneToMany &withForeignKey(std::string name) { foreignKey = name; return *this; } //default to '<name>Id'
	};

	struct ManyToMany
	{
		std::string name;
		std::string key;
		std::string foreignKey;
		std::string linkingTable;

		ManyToMany(const std::string name) : name(name) {}
		ManyToMany &withKey(std::string name) { key = name; return *this; } //default to 'id'
		ManyToMany &withForeignKey(std::string name) { foreignKey = name; return *this; } //default to '<name>Id'
		ManyToMany &withLinkingTable(std::string name) { linkingTable = name; return *this; } 
	};

	RelationMap &addMember(OneToOne r) { normalize(r); oneToOneMembers.push_back(r); return *this; }
	RelationMap &addMember(OneToMany r) { normalize(r); oneToManyMembers.push_back(r); return *this; }
	RelationMap &addMember(ManyToMany r) { normalize(r); manyToManyMembers.push_back(r); return *this; }

	RelationMap &requireOwner(OneToOne r) { normalize(r); oneToOneOwners.push_back(r); return *this; }
	RelationMap &requireOwner(OneToMany r) { normalize(r); oneToManyOwners.push_back(r); return *this; }
	RelationMap &requireOwner(ManyToMany r) { normalize(r); manyToManyOwners.push_back(r); return *this; }

	std::vector<OneToOne> oneToOneMembers;
	std::vector<OneToMany> oneToManyMembers;
	std::vector<ManyToMany> manyToManyMembers;

	std::vector<OneToOne> oneToOneOwners;
	std::vector<OneToMany> oneToManyOwners;
	std::vector<ManyToMany> manyToManyOwners;

	protected:

	virtual std::string getDefaultKeyName();
	virtual std::string getDefaultForeignKeyName(const std::string tableName);
	virtual std::string getDefaultLinkingTableName(const std::string tableName1, const std::string tableName2);

	private:

	void normalize(OneToOne);
	void normalize(OneToMany);
	void normalize(ManyToMany);

	std::string name;
};

/* example ripped from print example
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
*/
#endif
