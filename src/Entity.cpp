#include <string>
#include <vector>
#include <memory>

#include "Entity.h"
#include "BackEnd.h"

bool Entity::save(BackEnd &backEnd, ErrorBinder &errors)
{
	validate(errors);
	if (!errors.hasErrors())
	{
		return false;
	}
	/*
	if (!backEnd->validate())
	{
		//todo use callback function
		return false;
	}

	bool transactionStartedHere = backEnd.beginTransaction();
	try {
		//callBeforeSaveFunctions();
		if (!isNew())
			backEnd.update(getId(), diff);
		else
			backEnd.insert(diff);
		//callAfterSaveFunctions();
	} catch(...) {
		if (transactionStartedHere)
			backEnd.rollback();
		throw;
	}
	if (transactionStartedHere)
		backEnd.commit();
	*/
	return true;
}

bool Entity::isNew()
{
	return true;
}
