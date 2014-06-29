#include<jsoncpp/json/json.h>
#include "Entity.h"
#include "EntityList.h"
#include "JsonError.h"

void JsonError::bind(Entity &parent, const std::string name, Entity &entity)
{
	auto obj = Json::Value(Json::objectValue);
	validate(entity, JsonError(obj));
	jsonVal[name] = obj;
}
void JsonError::bind(Entity &parent, const std::string name, EntityListBase &list)
{
	auto array = Json::Value(Json::arrayValue);
	for (int i = 0; i < list.size(); ++i)
	{
		auto element = Json::Value(Json::objectValue);
		validate(list.getRef(i), JsonError(element));
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

void validate(Entity &entity, JsonError e)
{
	entity.validate(e);
	entity.bindMembers(e);
}
