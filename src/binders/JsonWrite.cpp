#include<stdexcept>
#include<jsoncpp/json/json.h>
#include "../Entity.h"
#include "../EntityList.h"
#include "JsonWrite.h"

std::string JsonWrite::toString(Entity &entity)
{
	Json::StyledWriter writer;
	Json::Value root;
	JsonWrite r(root);
	entity.bindMembers(r);
	return writer.write(root);
}
void JsonWrite::bind(Entity &parent, const std::string name, bool & field) 
{ 
	if (!diffMode() || parent.isDirty(field))
		jsonVal[name] = field; 
}
void JsonWrite::bind(Entity &parent, const std::string name, unsigned int & field) 
{ 
	if (!diffMode() || parent.isDirty(field))
		jsonVal[name] = field; 
}
void JsonWrite::bind(Entity &parent, const std::string name, int & field) 
{ 
	if (!diffMode() || parent.isDirty(field))
		jsonVal[name] = field; 
}
void JsonWrite::bind(Entity &parent, const std::string name, double & field) 
{ 
	if (!diffMode() || parent.isDirty(field))
		jsonVal[name] = field; 
}
void JsonWrite::bind(Entity &parent, const std::string name, std::string & field) 
{ 
	if (!diffMode() || parent.isDirty(field))
		jsonVal[name] = field; 
}
void JsonWrite::bind(Entity &parent, const std::string name, Entity &entity)
{
	auto obj = Json::Value(Json::objectValue);
	JsonWrite writer(obj, diffMode());
	entity.bindMembers(writer);
	if (obj.size())
		jsonVal[name] = obj;
}
void JsonWrite::bind(Entity &parent, const std::string name, EntityListBase &list)
{
	auto array = Json::Value(Json::arrayValue);
	for (int i = 0; i < list.size(); ++i)
	{
		auto element = Json::Value(Json::objectValue);
		JsonWrite writer(element, diffMode());
		list.getRef(i).bindMembers(writer);
		array.append(element);
	}
	if (array.size())
		jsonVal[name] = array;
}
