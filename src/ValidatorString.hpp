#ifndef VALIDATORSTRING_HPP
#define VALIDATORSTRING_HPP

#include <string>
#include "Regex.hpp"
#include "Validator.hpp"

template<class ValidationBinder, class Entity>
class ValidatorString : public Validator<ValidatorString<ValidationBinder, Entity>, std::string>
{
	public:

	ValidatorString(Entity &entity, std::string &field, ValidationBinder &e) : 
		Validator<ValidatorString, std::string>(entity, field, e) {}

	ValidatorString &minLen(const int l)
	{ 
		if (!this->isChainBroken() && this->field.size() < l) 
			this->addError("tooShort"); 
		return *this; 
	}
	ValidatorString &maxLen(const int l)
	{ 
		if (!this->isChainBroken() && this->field.size() > l) 
			this->addError("tooLong"); 
		return *this; 
	}
	//kind of violating single responsibility here
	ValidatorString &asEmail()
	{ 
		if (!this->isChainBroken())
			this->matches("^[a-zA-Z0-9\\-_\\.\\+]+@[a-zA-Z0-9_\\-]+(\\.[a-zA-Z0-9_\\-]+)*$"); 
		return *this; 
	}
	ValidatorString &allDigits()
	{ 
		if (!this->isChainBroken())
			this->matches("^[0-9]*$"); 
		return *this; 
	}
	ValidatorString &matches(const std::string reg, const std::string errorToken = "invalid")
	{ 
		if (!this->isChainBroken() && !Regex::match(reg, this->field))
			this->addError(errorToken); 
	}

	bool isBlank()
	{ 
		return this->field.size() > 0;	
	}
};

//ValidatorTraits specialization
template<class ValidationBinderType, class EntityType>
struct ValidatorTraits<ValidatorString<ValidationBinderType, EntityType>>
{
	typedef ValidationBinderType ValidationBinder;
	typedef EntityType Entity;
};

#endif
