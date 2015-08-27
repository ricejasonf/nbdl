//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<stdexcept>
#include "ApplyDiff.hpp"

namespace nbdl {
namespace binders {
namespace jsoncpp {

ApplyDiff::ApplyDiff(const Json::Value &value) :
	jsonVal(value)
{
	if (!jsonVal.isObject())
		throw std::runtime_error("JSON Object expected");
}

void ApplyDiff::bind(const std::string name, bool &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isBool())
		throw std::runtime_error("JSON Boolean expected");
	field = obj.asBool();
}

void ApplyDiff::bind(const std::string name, unsigned int &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asUInt();
}

void ApplyDiff::bind(const std::string name, int &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isIntegral())
		throw std::runtime_error("JSON Integral expected");
	field = obj.asInt();
}

void ApplyDiff::bind(const std::string name, double &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isNumeric())
		throw std::runtime_error("JSON Number expected");
	field = obj.asDouble();
}

void ApplyDiff::bind(const std::string name, std::string &field)
{
	if (!jsonVal.isMember(name))
		return;
	const Json::Value &obj = jsonVal[name];
	if (!obj.isString())
		throw std::runtime_error("JSON String expected");
	field = obj.asString();
}

ApplyDiff ApplyDiff::createObjectReader(const std::string name)
{
	const Json::Value &obj = jsonVal[name];
	if (!obj.isObject())
		throw std::runtime_error("JSON Object expected");
	return ApplyDiff(obj);
}

}//jsoncpp
}//binders
}//nbdl
