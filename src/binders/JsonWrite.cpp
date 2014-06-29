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
	auto obj = Json::Value(Json::objectValue);
	entity.bindMembers(JsonWrite(obj, isDiffMode()))
	if (obj.size())
		jsonVal[name] = obj;
}
void JsonWrite::bind(Entity &parent, const std::string name, EntityListBase &list)
{
	auto array = Json::Value(Json::arrayValue);
	for (int i = 0; i < list.size(); ++i)
	{
		auto element = Json::Value(Json::objectValue);
		list.getRef(i).bindMembers(JsonWrite(element, isDiffMode()));
		array.append(element);
	}
	if (array.size())
		jsonVal[name] = array;
}
