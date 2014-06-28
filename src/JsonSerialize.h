#ifndef JSONSERIALIZE_H
#define JSONSERIALIZE_H

#include"Entity.h"

class JsonSerialize : public Entity::Binder
{
	public:

	JsonSerialize(Json::Value &value) :
		jsonVal(value) {}

	static std::string toString(Entity &entity);

	template<typename T>
	void bind(const std::string name, T &value) { jsonVal[name] = value; }

	void bind(const std::string name, Entity &entity);
	void bind(const std::string name, EntityListBase &list);
			
	private: 
	
	Json::Value &jsonVal;
		
};

#endif
