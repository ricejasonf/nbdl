#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "MemberSet.hpp"
#include "ValidatorNumber.hpp"
#include "ValidatorString.hpp"

namespace nbdl {

class EntityBase {};

template<class Impl>
class Entity : public EntityBase
{
	Impl *impl() 
	{
		return static_cast<Impl *>(this);
	}

	//friend class EntityMutable<Impl>;
	template<typename T>
	uintptr_t memberOffset(T &member)
	{
		return (uintptr_t)&member - (uintptr_t)impl();
	}

	public:

	template<class ValidationBinder>
	void runValidation(ValidationBinder &binder)
	{
		validate(binder);
	}

	template<class Binder>
	void bindMembers(Binder &binder)
	{
		impl()->members(binder);
	}

	//non mutable entities are considered dirty
	bool isDirty(void)
	{
		return true;
	}
	template<typename T>
	bool isDirty(T &field)
	{
		return true;
	}

	protected:

	template<class ValidationBinder, typename T>
	ValidatorNumber<ValidationBinder, Entity, T> 
		validateNumber(ValidationBinder &e, T &field)
	{ 
		return ValidatorNumber<ValidationBinder, Entity, T>(*this, field, e);
	}

	template<class ValidationBinder>
	ValidatorString<ValidationBinder, Entity> 
		validateString(ValidationBinder &e, std::string &field)
	{ 
		return ValidatorString<ValidationBinder, Entity>(*this, field, e);
	}

	template<class Binder, typename T>
	void member(Binder &b, const std::string name, T &field)
	{ 
		b.bindMember(name, field); 
	}
};

} //nbdl
#endif
