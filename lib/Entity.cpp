#include "Entity.h"

const string Entity::get(const string name)
{
	/* 
	 * i decided that returning the diff value when
	 * set was useless and i wanted to avoid echoing
	 * unfiltered or invalid data
	 */
	ValueMap::iterator i;
	i = values.find(name);
	if (i != values.end())
		return *i;
	return string("");
}

void Entity::save()
{
	//todo handle relations
	clearValidators();
	validate();
	changedValues.clear();

	bool transactionStartedHere = backend.beginTransaction();
	try {
		//callBeforeSaveFunctions();
		if (getId())
			backEnd.insert(getDiff());
		else
			backEnd.update(getId(), getDiff());
		//callAfterSaveFunctions();
	} catch(...) {
		if (transactionStartedHere)
			backend.rollBack();
		throw;
	}
	if (transactionStartedHere)
		backend.commit();
}

void Validator::addError(const string name, const string error)
{
	vector<string> &errors = entity.errors[name];
	if (errors.find(errors.begin(), errors.end(), error) == errors.end())
		errors.push_back(error);
}

<template class V>
V &Entity::validate<V>(const string &name)
{
	ValueMap::iterator i;
	V *v;
	i = changedValues.find(name);
	if (i != changedValues.end())
		v = new V(*this, *i);
	else
		v = new V(*this);
	validators.push_back(v);
	return *v;
}

void Entity::clearValidators() 
{
	/* trying out ranged based for loop
	vector<Validator *>::iterator i;
	for (i = validators.begin(); i != validators.end(); ++validators)
		delete *i;
	*/
	for(auto &i : validators)
		delete i;
	//make sure we clear to prevent double deletes
	validators.clear();
}
