#ifndef JSONSERIALIZE_H
#define JSONSERIALIZE_H

#include"Entity.h"

class JsonSerialize : public Entity::Binder
{
	public:

	static std::string toString(Entity &entity);

	protected:

	void serialize(Entity &entity, Json::Value &obj);

	//the bind methods are protected to prevent the null pointer abuse
	friend class Entity;
	void bind(const std::string name, Entity &entity);
	void bind(const std::string name, EntityListBase &list);
			
	private: 
	
	Json::Value *currentObj;
		
};

#endif
