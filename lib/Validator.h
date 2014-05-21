#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Entity;

class Validator
{
	public:

	Validator(Entity &entity, const std::string &name);
	virtual ~Validator() {}
	inline bool hasValue() { return has_value; }
	inline std::string getValue() { return value; }
	inline std::string getName() { return name; }

	protected:

	void addError(const std::string &error);
	void _matches(const std::string &r, const std::string &token = "invalid");
	inline void _optional() { if (!hasValue()) chain_broken = true; }
	inline bool isBlank() { return value.size() < 1; }

	void _required();
	std::string value;
	bool chain_broken;

	private:

	Entity &entity;
	const std::string name;
	bool has_value;
};

#endif
