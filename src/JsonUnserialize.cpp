#include<stdexcept>
#include<jsoncpp/json/json.h>
#include "JsonUnserialize.h"
#include "Entity.h"
#include "EntityList.h"

void JsonUnserialize::fromString(std::string &json, Entity &entity)
{
	Json::Reader reader;
	Json::Value root;
	JsonUnserialize r;
	if (!reader.parse(json, root, false))
	{
		throw std::runtime_error("JSON parse error");
	}
	r.unserialize(entity, root);
}

void JsonUnserialize::unserialize(Entity &entity, const Json::Value &obj)
{
	currentObj = &obj;
	if (!obj.isObject())
	{
		throw std::runtime_error("JSON Object expected");
		return;
	}
	for (auto itr = obj.begin(); itr != obj.end(); ++itr)
	{
		auto &i = *itr;
		if (i.isString())
		{
			initValue(entity, itr.key().asString(), i.asString());
		}
		else if (i.isInt())
		{
			initValue(entity, itr.key().asString(), std::to_string(i.asInt()));
		}
		else if (i.isDouble())
		{
			initValue(entity, itr.key().asString(), std::to_string(i.asDouble()));
		}
		else if (i.isBool())
		{
			initValue(entity, itr.key().asString(), std::to_string(i.asBool()));
		}
	}
	bindMembers(entity);
	currentObj = NULL;
}

void JsonUnserialize::bind(const std::string name, bool &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isBool())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asBool();
}

void JsonUnserialize::bind(const std::string name, unsigned int &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asUInt();
}

void JsonUnserialize::bind(const std::string name, int &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asInt();
}

void JsonUnserialize::bind(const std::string name, double &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isNumeric())
		throw std::runtime_error("JSON Number expected");
	field = obj.asDouble();
}

void JsonUnserialize::bind(const std::string name, Entity &entity)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isObject())
		throw std::runtime_error("JSON Object expected");
	entity.bindMembers(JsonUnserialize(obj));
}

void JsonUnserialize::bind(const std::string name, EntityListBase &list)
{
	const Json::Value &obj = *currentObj;
	const Json::Value &array = obj[name];
	if (!array.isArray())
	{
		throw std::runtime_error("JSON Array expected");
		return;
	}
	list.initWithSize(array.size());
	auto refs = list.getEntityRefs();
	auto refItr = refs.begin();

	for (auto &i : array)
	{
		unserialize(*refItr, i);
		++refItr;
	}
	currentObj = &obj;
}
