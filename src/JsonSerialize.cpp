#include<stdexcept>
#include<jsoncpp/json/json.h>
#include "Entity.h"
#include "EntityList.h"
#include "JsonSerialize.h"

std::string JsonSerialize::toString(Entity &entity)
{
	Json::StyledWriter writer;
	Json::Value root;
	JsonSerialize r(root);
	entity.bindMembers(r);
	return writer.write(root);
}
void JsonSerialize::bind(const std::string name, Entity &entity)
{
	auto subVal = Json::Value(Json::objectValue);
	entity.bindMembers(JsonSerialize(subVal));
	jsonVal[name] = subVal;
}
void JsonSerialize::bind(const std::string name, EntityListBase &list)
{
	auto array = Json::Value(Json::arrayValue);
	subBinder.bindMembers(entity);
	for (auto &entity : list.getEntityRefs())
	{
		auto element = Json::Value(Json::objectValue);
		entity.bindMembers(JsonSerialize(element));
		array.append(element);
	}
	jsonVal[name] = array;
}
