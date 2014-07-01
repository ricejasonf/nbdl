#ifndef ERRORBINDER_H
#define ERRORBINDER_H

#include<string>
#include<vector>
#include<unordered_map>
#include "Binder.h"
#include<stdint.h>
class Entity;

class ErrorBinder : public Binder
{
	public:

	ErrorBinder() : _hasErrors(false) {}

	template<typename T>
	void addError(Entity &entity, T &field, const std::string error)
	{
		_hasErrors = true;
		errors[(uintptr_t)&field - (uintptr_t)&entity].push_back(error);
	}
	bool hasErrors() { return _hasErrors; }

	protected:

	template<typename T>
	std::vector<std::string> getErrors(Entity &entity, T &field)
	{
		return errors[(uintptr_t)&field - (uintptr_t)&entity];
	}

	void validateChild(Entity &entity, ErrorBinder &e);

	private:

	bool _hasErrors;
	std::unordered_map<uintptr_t, std::vector<std::string>> errors;
};

#endif
