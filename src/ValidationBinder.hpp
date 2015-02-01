#ifndef VALIDATIONBINDER_HPP
#define VALIDATIONBINDER_HPP

#include<string>
#include<vector>
#include<unordered_map>
#include<stdint.h>
#include"Binder.hpp"

namespace nbdl {

template<class Impl>
class ValidationBinder : public Binder<Impl>
{
	bool hasErrors_;
	bool isUpdate_;
	std::unordered_map<uintptr_t, std::vector<std::string>> errors;

	public:

	ValidationBinder() : 
		hasErrors_(false),
		isUpdate_(false)
   	{}

	bool isUpdate() { return isUpdate_; }

	template<typename T>
	std::vector<std::string> getErrors(Entity &entity, T &field)
	{
		return errors[(uintptr_t)&field - (uintptr_t)&entity];
	}

	template<typename T>
	void addError(Entity &entity, T &field, const std::string error)
	{
		hasErrors_ = true;
		errors[(uintptr_t)&field - (uintptr_t)&entity].push_back(error);
	}
	bool hasErrors() { return hasErrors_; }

	protected:

	template<class EntityType, class ValidationBinderType>
	void validateChild(EntityType &entity, ValidationBinderType &e);
	{
		entity.runValidation(e);
		entity.bindMembers(e);
		hasErrors_ = hasErrors_ || e.hasErrors();
	}
};

}//nbdl

#endif
