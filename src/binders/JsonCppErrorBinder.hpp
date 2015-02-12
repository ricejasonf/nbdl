#ifndef NBDL_BINDERS_JSONCPPVALIDATIONERRORS_HPP
#define NBDL_BINDERS_JSONCPPVALIDATIONERRORS_HPP

#include<jsoncpp/json/json.h>

namespace nbdl {
namespace binders {

class JsonCppValidationErrors
{
	Json::Value &jsonVal;

	public: 
	
	JsonCppValidationErrors(const Json::Value &value)
		:value(value) {}

	JsonCppValidationErrors(
	void addError(ErroToken token);
}

}//binders
}//nbdl
#endif
