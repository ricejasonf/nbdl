//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <string>
#include <vector>
#include <memory>

#include "Entity.hpp"
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
