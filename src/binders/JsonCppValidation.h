#ifndef JSONCPPVALIDATION_H
#define JSONCPPVALIDATION_H

#include "ValidationBinder.hpp"

class Entity;
class EntityListBase;

class JsonCppValidation : public ValidationBinder<JsonCppValidation>
{
	public:

	JsonCppValidation(Json::Value &value) :
		jsonVal(value) {}

	template<typename T>
	void bind(Entity &parent, const std::string name, T &value);
	{ 
		writeErrors(getErrors(parent, value));
	}

	//todo needs sfinae for bases of Entity
	void bind(Entity &parent, const std::string name, Entity &entity);
	void bind(Entity &parent, const std::string name, EntityListBase &list);
			
	private: 

	void writeErrors(std::string name, std::vector<std::string> errors);
	void validate(Entity &, JsonError);
	
	Json::Value &jsonVal;
		
};

#endif
