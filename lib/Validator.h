#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "Entity.h"

class Validator
{
	public:

	Validator(Entity &entity, const string name);
	virtual ~Validator() {}
	inline bool hasValue() { return _hasValue; }

	protected:

	inline void addError(const string error) { entity.addError(name, error); chainBroken = true; }
	inline void _matches(const string r const string token = "invalid") 
	{ 
		if (!regex_match(value, r))
			addError(token); 
	}
	inline void _optional() { if (!hasValue()) chainBroken = true; }
	inline bool isBlank() { return value.size() < 1 }

	void _required();
	string value;
	bool chainBroken;

	private:

	Entity &entity;
	const string &name;
	bool _hasValue;
	bool _isRequired;
	vector<function> funcs;
}

#endif
