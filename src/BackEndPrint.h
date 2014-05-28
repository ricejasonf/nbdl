#ifndef BACKENDPRINTER_H
#define BACKENDPRINTER_H

#include "BackEnd.h"

class BackEndPrint : public BackEnd
{
	public:

	BackEndPrint(std::string entityName) : entityName(entityName) {}

	void insert(const ValueMap values);
	void update(int id, const ValueMap values);

	private:

	std::string entityName;
};

#endif
