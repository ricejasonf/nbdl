#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Entity;

class Validator
{
	public:

	Validator(Entity &entity, const std::string &name);
	virtual ~Validator() {}
	inline bool hasValue() { return _hasValue; }
	inline std::string getValue() { return value; }
	inline std::string getName() { return name; }

	protected:

	void addError(const std::string &error);
	void _matches(const std::string &r, const std::string &token = "invalid");
	inline void _optional() { if (!hasValue()) chainBroken = true; }
	inline bool isBlank() { return value.size() < 1; }

	void _required();
	std::string value;
	bool chainBroken;

	private:

	Entity &entity;
	const std::string name;
	bool _hasValue;
	bool _isRequired;
};

#endif
