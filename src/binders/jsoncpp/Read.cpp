//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<stdexcept>
#include "Read.hpp"

namespace nbdl {
namespace binders {
namespace jsoncpp {

void read::bind_(const Json::Value& obj, bool &field)
{
  if (!obj.isBool())
    throw std::runtime_error("JSON Boolean expected");
  field = obj.asBool();
}

void read::bind_(const Json::Value& obj, unsigned int &field)
{
  if (!obj.isIntegral())
    throw std::runtime_error("JSON Integral expected");
  field = obj.asUInt();
}

void read::bind_(const Json::Value& obj, int &field)
{
  if (!obj.isIntegral())
    throw std::runtime_error("JSON Integral expected");
  field = obj.asInt();
}

void read::bind_(const Json::Value& obj, double &field)
{
  if (!obj.isNumeric())
    throw std::runtime_error("JSON Number expected");
  field = obj.asDouble();
}

void read::bind_(const Json::Value& obj, std::string &field)
{
  if (!obj.isString())
    throw std::runtime_error("JSON String expected");
  field = obj.asString();
}

}//jsoncpp
}//binders
}//nbdl
