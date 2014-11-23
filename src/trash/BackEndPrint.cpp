#include <iostream>
#include "BackEndPrint.h"

using std::endl;
using std::cout;

void BackEndPrint::insert(const ValueMap values)
{
	cout << "INSERT to " << entityName << endl;
	for (auto &pair : values)
	{
		cout << "\t" << pair.first << ": " << pair.second << endl;
	}
}

void BackEndPrint::update(int id, const ValueMap values)
{
	cout << "UPDATE to " << entityName << " WHERE Id = " << id << endl;
	for (auto &pair : values)
	{
		cout << "\t" << pair.first << ": " << pair.second << endl;
	}
}
