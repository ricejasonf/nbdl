#ifndef JSONUNSERIALIZE_H
#define JSONUNSERIALIZE_H

#include"Entity.h"
namespace Json { class Value; }


class JsonUnserialize : public Entity::Binder
{
	public:

	static void fromString(std::string &json, Entity &entity);

	void unserialize(Entity &entity, const Json::Value &obj);

	protected:

	//the bind methods are protected to prevent null pointer abuse
	friend class Entity;
	void bind(const std::string name, Entity &entity);
	void bind(const std::string name, EntityListBase &list);
			
	private: 
	
	const Json::Value *currentObj;
		
};

#endif
