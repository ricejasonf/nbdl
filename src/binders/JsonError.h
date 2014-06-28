#ifndef JSONERROR_H
#define JSONERROR_H

class Entity;
class EntityListBase;

class JsonError : public ErrorBinder
{
	public:

	JsonError(Json::Value &value) :
		jsonVal(value) {}

	template<typename T>
	void bind(Entity &parent, const std::string name, T &value);
	{ 
		writeErrors(getErrors(parent, value));
	}

	void bind(Entity &parent, const std::string name, Entity &entity);
	void bind(Entity &parent, const std::string name, EntityListBase &list);
			
	private: 

	void writeErrors(std::string name, std::vector<std::string> errors);
	
	Json::Value &jsonVal;
		
};

#endif
