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

void Entity::save()
{
	//todo handle relations
	validators.clear();
	validate();
	changedValues.clear();
	flushValidatedValues();
	backEnd->validate();
	flushValidatedValues();

	bool transactionStartedHere = backEnd->beginTransaction();
	try {
		//callBeforeSaveFunctions();
		if (getId())
			backEnd->insert(changedValues);
		else
			backEnd->update(getId(), changedValues);
		//callAfterSaveFunctions();
	} catch(...) {
		if (transactionStartedHere)
			backEnd->rollback();
		throw;
	}
	if (transactionStartedHere)
		backEnd->commit();
}

void Entity::addError(const std::string &name, const std::string &error)
{
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
