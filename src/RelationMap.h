#ifndef RELATIONMAP_H
#define RELATIONMAP_H

#include<string>
#include"RelationMap.h"

class RelationMap
{
	public:

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

	RelationMap &add(OneToOne r) { oneToOneList.push_back(r); return *this; }
	RelationMap &add(OneToMany r) { oneToManyList.push_back(r); return *this; }
	RelationMap &add(ManyToMany r) { manyToManyList.push_back(r); return *this; }

	protected:

	virtual std::string getDefaultKeyName();
	virtual std::string getDefaultForeignKeyName(const std::string tableName);
	virtual std::string getDefaultLinkingTableName(const std::string tableName1, const std::string tableName2);

	private:

	std::vector<OneToOne> oneToOneList;
	std::vector<OneToMany> manyToOneList;
	std::vector<ManyToMany> manyToManyList;
};

#endif
