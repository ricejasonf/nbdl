#include<stdexcept>
#include<jsoncpp/json/json.h>
#include "Entity.h"
#include "JsonSerialize.h"

std::string JsonSerialize::toString(Entity &entity)
{
	Json::StyledWriter writer;
	Json::Value root;
	JsonSerialize r;
	r.serialize(entity, root);
	return writer.write(root);
}

void JsonSerialize::serialize(Entity &entity, Json::Value &obj)
{
	currentObj = &obj;
	for (auto &i : getValues(entity))
	{
		obj[i.first] = i.second;
	}
	bindMembers(entity);
	currentObj = NULL;
}

void JsonSerialize::bind(const std::string name, Entity &entity)
{
	if (currentObj == NULL)
		throw std::runtime_error("Bind method called in wrong context");
	Json::Value &obj = *currentObj;
	obj[name] = Json::Value(Json::objectValue);
	serialize(entity, obj[name]);
	currentObj = &obj;
}

void JsonSerialize::bind(const std::string name, std::vector<Entity> &list)
{
	Json::Value &tempObj = *currentObj;
	Json::Value &array = tempObj[name] = Json::Value(Json::arrayValue);

	for (auto &i : list)
	{
		Json::Value element = array.append(Json::Value(Json::objectValue));
		serialize(i, element);
	}
	currentObj = &tempObj;
}
