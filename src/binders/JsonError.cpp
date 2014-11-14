#include<jsoncpp/json/json.h>
#include "../Entity.hpp"
#include "../EntityList.h"
#include "JsonError.h"

void JsonError::bind(Entity &parent, const std::string name, Entity &entity)
{
	auto obj = Json::Value(Json::objectValue);
	JsonError error(obj);
	validateChild(entity, error);
	jsonVal[name] = obj;
}
void JsonError::bind(Entity &parent, const std::string name, EntityListBase &list)
{
	auto array = Json::Value(Json::arrayValue);
	for (int i = 0; i < list.size(); ++i)
	{
		auto element = Json::Value(Json::objectValue);
		JsonError error(element);
		validateChild(list.getRef(i), error);
		array.append(element);
	}
	jsonVal[name] = array;
}

void JsonError::writeErrors(std::string name, std::vector<std::string> errors)
{
	if (!errors.size())
		return;
	auto array = Json::Value(Json::arrayValue);
	for (auto token: errors)
		array.append(token);
	jsonVal[name] = array;
}
