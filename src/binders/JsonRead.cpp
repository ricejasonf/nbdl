#include<stdexcept>
#include "JsonRead.h"

JsonRead::JsonRead(const Json::Value &value) :
	jsonVal(value)
{
	if (!jsonVal.isObject())
		throw std::runtime_error("JSON Object expected");
}

void JsonRead::bind(const std::string name, bool &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isBool())
		throw std::runtime_error("JSON Boolean expected");
	field = obj.asBool();
}

void JsonRead::bind(const std::string name, unsigned int &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asUInt();
}

void JsonRead::bind(const std::string name, int &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asInt();
}

void JsonRead::bind(const std::string name, double &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isNumeric())
		throw std::runtime_error("JSON Number expected");
	field = obj.asDouble();
}

void JsonRead::bind(const std::string name, std::string &field)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isString())
		throw std::runtime_error("JSON String expected");
	field = obj.asString();
}

JsonRead JsonRead::createObjectReader(const std::string name)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isObject())
		throw std::runtime_error("JSON Object expected");
	return JsonRead(obj);
}

/* i'm getting rid of 'lists' for now but will need this code
void JsonRead::bind(const std::string name, EntityListBase &list)
{
	const Json::Value &array = jsonVal[name];
	if (!array.isArray())
		throw std::runtime_error("JSON Array expected");
	list.initWithSize(array.size());
	int listSize = list.size();
	int i = 0;
	for (auto &obj : array)
	{
		JsonRead reader(obj);
		list.getRef(i++).bindMembers(reader);
		if (i >= listSize)
			break;
	}
}
*/
