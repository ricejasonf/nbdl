#include<stdexcept>
#include "JsonCppApplyDiff.hpp"

namespace nbdl {

JsonCppApplyDiff::JsonCppApplyDiff(const Json::Value &value) :
	jsonVal(value)
{
	if (!jsonVal.isObject())
		throw std::runtime_error("JSON Object expected");
}

void JsonCppApplyDiff::bind(const std::string name, bool &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isBool())
		throw std::runtime_error("JSON Boolean expected");
	field = obj.asBool();
}

void JsonCppApplyDiff::bind(const std::string name, unsigned int &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asUInt();
}

void JsonCppApplyDiff::bind(const std::string name, int &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asInt();
}

void JsonCppApplyDiff::bind(const std::string name, double &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isNumeric())
		throw std::runtime_error("JSON Number expected");
	field = obj.asDouble();
}

void JsonCppApplyDiff::bind(const std::string name, std::string &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isString())
		throw std::runtime_error("JSON String expected");
	field = obj.asString();
}

JsonCppApplyDiff JsonCppApplyDiff::createObjectReader(const std::string name)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isObject())
		throw std::runtime_error("JSON Object expected");
	return JsonCppApplyDiff(obj);
}

}//nbdl
