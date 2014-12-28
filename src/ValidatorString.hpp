#ifndef VALIDATORSTRING_HPP
#define VALIDATORSTRING_HPP

#include <string>
#include "Validator.hpp"

template<class ValidationBinder, class Entity>
class ValidatorString : public Validator<ValidatorString<ValidationBinder, EntityType>, std::string>
{
	public:

	ValidatorString(Entity &entity, std::string &field, ValidationBinder &e) : 
		Validator<ValidatorString, std::string>(entity, field, e) {}
	ValidatorString &minLen(const int)
	{ 
		if (!isChainBroken() && field.size() < l) 
			addError("tooShort"); 
		return *this; 
	}
	ValidatorString &maxLen(const int)
	{ 
		if (!isChainBroken() && field.size() > l) 
			addError("tooLong"); 
		return *this; 
	}
	//kind of violating single responsibility here
	ValidatorString &asEmail()
	{ 
		if (!isChainBroken())
			matches("^[a-zA-Z0-9\\-_\\.\\+]+@[a-zA-Z0-9_\\-]+(\\.[a-zA-Z0-9_\\-]+)*$"); 
		return *this; 
	}
	ValidatorString &allDigits()
	{ 
		if (!isChainBroken())
			matches("^[0-9]*$"); 
		return *this; 
	}
	#ifdef EMSCRIPTEN
	#include <emscripten.h>
	ValidatorString &matches(const std::string reg, const std::string errorToken = "invalid")
	{ 
		int match = EM_ASM_INT({
			var value = Pointer_stringify($0);
			var regStr = Pointer_stringify($1);
			return value.match(new RegExp(regStr)) !== null;
		}, field.c_str(), r.c_str());
		if (!isChainBroken() && !match)
			addError(token); 
	}
	#else
	#include <boost/regex.hpp>
	ValidatorString &matches(const std::string reg, const std::string errorToken = "invalid")
	{ 
		if (!isChainBroken() 
				&& !boost::regex_match(field, boost::regex(r, boost::regex::ECMAScript)))
			addError(token); 
	}
	#endif //ifdef EMSCRIPTEN

	bool isBlank()
	{ 
		return field.size() > 0;	
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
