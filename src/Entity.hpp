#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "ValidatorNumber.hpp"
#include "ValidatorString.hpp"

template<class Impl>
class Entity
{
	Impl *impl() 
	{
		return static_cast<Impl *>(this);
	}

	public:

	void runValidation(ValidationBinder &binder)
	{
		validate(binder);
	}

	template<class Binder>
	void bindMembers(Binder &binder)
	{
		impl()->members(binder);
	}

	protected:

	template<class ValidationBinder, typename T>
	ValidatorNumber<T> validateNumber(ValidationBinder &e, T &field)
	{ 
		return ValidatorNumber<ValidationBinder, T>(*this, field, e);
	}

	template<class ValidationBinder>
	ValidatorString validateString(ValidationBinder &e, std::string &field)
	{ 
		return ValidatorString<ValidationBinder>(*this, field, e);
	}

	template<class Binder, typename T>
	void member(Binder &b, const std::string name, T &field)
	{ 
		b.bindMember(*this, name, field); 
	}
};

#endif
