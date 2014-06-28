#include<jsoncpp/json/json.h>
#include "Entity.h"
#include "EntityList.h"
#include "JsonError.h"

void JsonError::bind(Entity &parent, const std::string name, Entity &entity)
{
	auto subVal = Json::Value(Json::objectValue);
	entity.bindMembers(JsonError(subVal))
	jsonVal[name] = subVal;
}
void JsonError::bind(Entity &parent, const std::string name, EntityListBase &list)
{
	auto array = Json::Value(Json::arrayValue);
	for (int i = 0; i < list.size(); ++i)
	{
		auto element = Json::Value(Json::objectValue);
		list.getRef(i).bindMembers(JsonError(element));
		array.append(element);
	}
	jsonVal[name] = array;
}

void JsonError:writeErrors(std::string name, std::vector<std::string> errors)
{
	if (!errors.size())
		return;
	auto array = Json::Value(Json::arrayValue);
	for (auto token: errors)
		array.append(token);
	jsonVal[name] = array;
}

