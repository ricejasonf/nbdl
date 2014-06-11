#ifndef JSONUNSERIALIZE_H
#define JSONUNSERIALIZE_H

#include "Binder.h"
#include "Entity.h"

class JsonUnserialize : public Binder
{
	public:

	static void run(std::string &json, Entity &entity);

	protected:

	void unserialize(Entity &entity, const Json::Value &obj);

	//the bind methods are protected to prevent the abuse of currentObj before it is set
	friend class Entity;
	void bind(const std::string name, Entity &entity);
	void bind(const std::string name, std::vector<Entity> &list);
			
	private: 
	
	//private constructor
	void JsonUnserialize();

	Json::Value *currentObj;
		
}

#endif
