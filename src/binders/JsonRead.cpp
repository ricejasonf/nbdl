#include<stdexcept>
#include<jsoncpp/json/json.h>
#include "JsonRead.h"
#include "Entity.h"
#include "EntityList.h"

JsonRead(Json::Value &value, bool diffMode) :
	jsonVal(value), Binder(diffMode) 
{
	if (!jsonVal.isObject())
		throw std::runtime_error("JSON Object expected");
}

void JsonRead::fromString(std::string &json, Entity &entity)
{
	Json::Reader reader;
	Json::Value root;
	JsonRead r(root);
	if (!reader.parse(json, root, false))
	{
		throw std::runtime_error("JSON parse error");
	}
	entity.bindMembers(JsonRead(root));
}

void JsonRead::bind(Entity &parent, const std::string name, bool &field)
{
	if (diffMode() && !jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isBool())
		throw std::runtime_error("JSON Boolean expected");
	if (diffMode())
		set(obj.asBool(), field);
	else
		field = obj.asBool();
}

void JsonRead::bind(Entity &parent, const std::string name, unsigned int &field)
{
	if (diffMode() && !jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	if (diffMode())
		set(obj.asUInt(), field);
	else
		field = obj.asUInt();
}

void JsonRead::bind(Entity &parent, const std::string name, int &field)
{
	if (diffMode() && !jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	if (diffMode())
		set(obj.asInt(), field);
	else
		field = obj.asInt();
}

void JsonRead::bind(Entity &parent, const std::string name, double &field)
{
	if (diffMode() && !jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isNumeric())
		throw std::runtime_error("JSON Number expected");
	if (diffMode())
		set(obj.asDouble(), field);
	else
		field = obj.asDouble();
}

void JsonRead::bind(Entity &parent, const std::string name, Entity &entity)
{
	if (diffMode() && !jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isObject())
		throw std::runtime_error("JSON Object expected");
	entity.bindMembers(JsonRead(obj));
}

void JsonRead::bind(Entity &parent, const std::string name, EntityListBase &list)
{
	const Json::Value &array = jsonVal[name];
	if (!array.isArray())
		throw std::runtime_error("JSON Array expected");
	list.initWithSize(array.size());
	int i = 0;
	for (auto &obj : array)
	{
		list.getRef(i++).bindMembers(JsonRead(obj));	
	}
}
