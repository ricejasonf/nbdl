#include<stdexcept>
#include "JsonWrite.h"

//todo replace this stuff with a template
void JsonWrite::bind(const std::string name, bool & field) 
{ 
	jsonVal[name] = field; 
}
void JsonWrite::bind(const std::string name, unsigned int & field) 
{ 
	jsonVal[name] = field; 
}
void JsonWrite::bind(const std::string name, int & field) 
{ 
	jsonVal[name] = field; 
}
void JsonWrite::bind(const std::string name, double & field) 
{ 
	jsonVal[name] = field; 
}
void JsonWrite::bind(const std::string name, std::string & field) 
{ 
	jsonVal[name] = field; 
}
/* 
void JsonWrite::bind(const std::string name, EntityListBase &list)
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
*/
