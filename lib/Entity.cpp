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

bool Entity::save()
{
	//todo handle relations
	has_errors = false;
	validators.clear();
	validate();
	if (has_errors)
	{
		//todo use callback function
		return false;
	}
	changedValues.clear();
	flushValidatedValues();
	backEnd->validate();
	if (has_errors)
	{
		//todo use callback function
		return false;
	}

	bool transactionStartedHere = backEnd->beginTransaction();
	try {
		//callBeforeSaveFunctions();
		if (getId())
			backEnd->update(getId(), changedValues);
		else
			backEnd->insert(changedValues);
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

void Entity::flushValidatedValues()
{
	for (auto &v : validators)
	{
		if (v->hasValue())
			changedValues[v->getName()] = v->getValue();
	}	
	validators.clear();
}

/*
template<class V>
V &Entity::validate(const std::string &name)
{
	V *v = new V(*this, name);
	validators.push_back(std::unique_ptr<Validator>(v));
	//return reference for idiom
	return *v;
}
*/
