#include<stdexcept>
#include<jsoncpp/json/json.h>
#include "JsonRead.h"
#include "Entity.h"
#include "EntityList.h"

JsonRead(Json::Value &value) :
	jsonVal(value) 
{
	if (!jsonVal.isObject())
		throw std::runtime_error("JSON Object expected");
}

void JsonRead::fromString(std::string &json, Entity &entity)
{
	Json::Reader reader;
	Json::Value root;
	JsonRead r;
	if (!reader.parse(json, root, false))
	{
		throw std::runtime_error("JSON parse error");
	}
	r.unserialize(entity, root);
}

void JsonRead::bind(const std::string name, bool &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isBool())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asBool();
}

void JsonRead::bind(const std::string name, unsigned int &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asUInt();
}

void JsonRead::bind(const std::string name, int &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asInt();
}

void JsonRead::bind(const std::string name, double &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isNumeric())
		throw std::runtime_error("JSON Number expected");
	field = obj.asDouble();
}

void JsonRead::bind(const std::string name, Entity &entity)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isObject())
		throw std::runtime_error("JSON Object expected");
	entity.bindMembers(JsonRead(obj));
}

void JsonRead::bind(const std::string name, EntityListBase &list)
{
	const Json::Value &array = jsonVal[name];
	if (!array.isArray())
		throw std::runtime_error("JSON Array expected");
	list.initWithSize(array.size());
	auto refs = list.getEntityRefs();
	auto refItr = refs.begin();

	for (auto &obj : array)
	{
		*refItr.bindMembers(JsonRead(obj));	
		++refItr;
	}
}
