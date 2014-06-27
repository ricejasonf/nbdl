#include <string>
#include <vector>
#include <memory>

#include "Entity.h"

const std::string Entity::get(const std::string &name)
{
	/* 
	 * i decided that returning the diff value when
	 * set was useless and i wanted to avoid echoing
	 * unfiltered or invalid data
	 */
	ValueMap::iterator i;
	i = values.find(name);
	if (i != values.end())
		return (*i).second;
	return std::string("");
}

bool Entity::save(BackEnd &backEnd, ErrorList &errors)
{
	//todo handle relations
	has_errors = false;
	ValueMap validatedDiff;
	validate(validatedDiff);
	diff = validatedDiff;
	if (has_errors)
	{
		//todo use callback function
		return false;
	}
	backEnd->validate();
	if (has_errors)
	{
		//todo use callback function
		return false;
	}

	bool transactionStartedHere = backEnd->beginTransaction();
	try {
		//callBeforeSaveFunctions();
		if (!isNew())
			backEnd->update(getId(), diff);
		else
			backEnd->insert(diff);
		//callAfterSaveFunctions();
	} catch(...) {
		if (transactionStartedHere)
			backEnd->rollback();
		throw;
	}
	if (transactionStartedHere)
		backEnd->commit();
	return true;
}

void Entity::addError(const std::string &name, const std::string &error)
{
	has_errors = true;
	std::vector<std::string> &_errors = errors[name];
	if (std::find(_errors.begin(), _errors.end(), error) == _errors.end())
		_errors.push_back(error);
}

/*
void Entity::bind(Binder &b, const std::string name, Entity &entity) { b.bind(name, entity); }
void Entity::bind(Binder &b, const std::string name, std::vector<Entity> &list) { b.bind(name, list); }
*/
