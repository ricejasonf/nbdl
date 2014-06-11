#ifndef BUILDERJSON_H
#define BUILDERJSON_H

#include<exception>
#include<json/json.h>
#include "Entity.h"

class JsonUnserialize : public Binder
{
	public:

	static void run(std::string &json, Entity &entity)
	{
		Json::Reader reader;
		Json::Value root;
		JsonUnserialize r;
		if (!reader.parse(json, root, false))
		{
			throw std::exception("JSON parse error");
		}
		r.unserialize(entity, root)
	}

	protected:

	void unserialize(Entity &entity, const Json::Value &obj)
	{
		currentObj = &obj;
		if (!obj.isObject())
		{
			throw std::exception("JSON Object expected");
			return;
		}
		for (auto &i : obj)
		{
			if (*i.isString())
			{
				initValue(entity, i.key(), *i);
			}
			else if (*i.isNumeric())
			{
				initValue(entity, i.key(), std::to_string(*i));
			}
		}
		entity.bindRelations(this);
	}

	//the bind methods are protected to prevent the abuse currentObj before it is set
	friend class Entity;

	void bind(const std::string name, Entity &entity)
	{
		Json::Value &obj = *currentObj;
		unserialize(entity, obj[name]);
		currentObj = &obj;
	}

	void bind(const std::string name, std::vector<Entity> &list)
	{
		JsonValue &tempObj = *currentObj;
		JsonValue &array = tempObj[name];
		if (!array.isArray())
		{
			throw std::exception("JSON Array expected");
			return;
		}
		list.resize(array.size());

		for (auto &i : array)
			unserialize(list[i.key()], *i);
		currentObj = &tempObj;
	}
			
	private: 
	
	//private constructor
	void JsonUnserialize();

	Json::Value *currentObj;
		
}

#endif
