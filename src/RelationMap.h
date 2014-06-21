#ifndef RELATIONMAP_H
#define RELATIONMAP_H

#include<string>
#include"RelationMap.h"

class RelationMap
{
	public:

	RelationMap() = delete;
	RelationMap(std::string name) : name(name) {}
	virtual compl RelationMap() {}

	struct OneToOne
	{
		std::string name;
		std::string key;
		std::string foreignKey;

		void OneToOne(const std::string name) : name(name) {}
		OneToOne &withKey(std::string name) { key = name; return *this; }
		OneToOne &withForeignKey(std::string name) { foreignKey = name; return *this; } //default to 'id'
	};

	struct OneToMany
	{
		std::string name;
		std::string key;
		std::string foreignKey;

		void OneToMany(const std::string name) : name(name) {}
		OneToMany &withKey(std::string name) { key = name; } //default to 'id'
		OneToMany &withForeignKey(std::string name) { foreignKey = name; return *this; } //default to '<name>Id'
	};

	struct ManyToMany
	{
		std::string name;
		std::string key;
		std::string foreignKey;
		std::string linkingTable;

		void ManyToMany(const std::string name) : name(name) {}
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
	std::vector<OneToMany> manyToOneMembers;
	std::vector<ManyToMany> manyToManyMembers;

	std::vector<OneToOne> oneToOneOwners;
	std::vector<OneToMany> manyToOneOwners;
	std::vector<ManyToMany> manyToManyOwners;

	protected:

	virtual std::string getDefaultKeyName();
	virtual std::string getDefaultForeignKeyName(const std::string tableName);
	virtual std::string getDefaultLinkingTableName(const std::string tableName1, const std::string tableName2);

	private:

	void normalize(OneToOne);
	void normalize(OneToMany);
	void normalize(ManyToMany);

};

#endif
