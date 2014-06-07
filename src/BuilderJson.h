#ifndef BUILDERJSON_H
#define BUILDERJSON_H

#include "Entity.h"

class BuilderJson
{
	public:

	void buildEntity(Entity &entity, Json::Value obj)
	{
		if (!obj.isObject())
			return entity;
		for (auto &i : obj)
		{
			if (*i.isString())
			{
				entity.set(i.key(), *i);
			}
			else if (*i.isNumeric())
			{
				entity.set(i.key(), std::to_string(*i));
			}
		}
			
	}

	//todo requires the creation of the object so use a template
	void buildEntityList(EntityList &list, Json::Value array)
	{
		if (!array.isArray())
			return list;
		for (auto &i : array)
		{
			
		
}

#endif
