#include<stdexcept>
#include<jsoncpp/json/json.h>
#include "JsonRead.h"
#include "../Entity.h"
#include "../EntityList.h"

JsonRead::JsonRead(const Json::Value &value, bool diffMode) :
	jsonVal(value), Binder(diffMode) 
{
	if (!jsonVal.isObject())
		throw std::runtime_error("JSON Object expected");
}

void JsonRead::fromString(std::string &json, Entity &entity)
{
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json, root, false))
	{
		throw std::runtime_error("JSON parse error");
	}
	JsonRead r(root);
	entity.bindMembers(r);
}

void JsonRead::bind(Entity &parent, const std::string name, bool &field)
{
	if (diffMode() && !jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isBool())
		throw std::runtime_error("JSON Boolean expected");
	if (diffMode())
		set(parent, obj.asBool(), field);
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
		set(parent, obj.asUInt(), field);
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
		set(parent, obj.asInt(), field);
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
		set(parent, obj.asDouble(), field);
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
	JsonRead reader(obj);
	entity.bindMembers(reader);
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
		JsonRead reader(obj);
		list.getRef(i++).bindMembers(reader);
	}
}
