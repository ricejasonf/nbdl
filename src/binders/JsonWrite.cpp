#include<stdexcept>
#include<jsoncpp/json/json.h>
#include "Entity.h"
#include "EntityList.h"
#include "JsonWrite.h"

std::string JsonWrite::toString(Entity &entity)
{
	Json::StyledWriter writer;
	Json::Value root;
	JsonWrite r(root);
	entity.bindMembers(r);
	return writer.write(root);
}
void JsonWrite::bind(Entity &parent, const std::string name, Entity &entity)
{
	auto subVal = Json::Value(Json::objectValue);
	entity.bindMembers(
		JsonWrite(subVal)
			.diffMode(isDiffMode()));
	jsonVal[name] = subVal;
}
void JsonWrite::bind(Entity &parent, const std::string name, EntityListBase &list)
{
	auto array = Json::Value(Json::arrayValue);
	subBinder.bindMembers(entity);
	for (auto &entity : list.getEntityRefs())
	{
		auto element = Json::Value(Json::objectValue);
		entity.bindMembers(
			JsonWrite(element)
				.diffMode(isDiffMode()));
		array.append(element);
	}
	jsonVal[name] = array;
}
