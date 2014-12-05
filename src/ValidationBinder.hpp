#ifndef VALIDATIONBINDER_HPP
#define VALIDATIONBINDER_HPP

#include<string>
#include<vector>
#include<unordered_map>
#include "Binder.h"
#include<stdint.h>
class Entity;

class ValidationBinder : public Binder
{
	public:

	ValidationBinder() : 
		hasErrors_(false),
		isUpdate_(false)
   	{}

	template<typename T>
	void addError(Entity &entity, T &field, const std::string error)
	{
		hasErrors_ = true;
		errors[(uintptr_t)&field - (uintptr_t)&entity].push_back(error);
	}
	bool hasErrors() { return hasErrors_; }
	bool isUpdate() { return isUpdate_; }

	protected:

	template<typename T>
	std::vector<std::string> getErrors(Entity &entity, T &field)
	{
		return errors[(uintptr_t)&field - (uintptr_t)&entity];
	}

	void validateChild(Entity &entity, ValidationBinder &e);

	private:

	bool isUpdate_;
	bool hasErrors_;
	std::unordered_map<uintptr_t, std::vector<std::string>> errors;
};

#endif
