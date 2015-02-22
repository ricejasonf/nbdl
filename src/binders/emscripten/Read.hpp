#ifndef NBDL_EMSCRIPTEN_READ_HPP
#define NBDL_EMSCRIPTEN_READ_HPP

#include<string>
#include<emscripten/val.h>

namespace nbdl {
namespace emscripten {

class Read
{
	//entities will be wrapped by javascript
	//side code to support duck typing of properties
	//to fail on invalid inputs
	emscripten::val &jsonVal;

	Read createObjectReader(const std::string name)
	{
		Read reader(jsonVal[name]);
		return reader;
	}

	public:

	Read(emscripten::val &val) :
		jsonVal(val) {}

	template<typename T>
	void bindMember(const std::string name, T &t)
	{
		//ideas
		jsonVal.call<void>("requiredString", name);
		jsonVal.call<void>("requiredIntegral", name);
		jsonVal.call<void>("requiredNumber", name);
		jsonVal.call<void>("requiredBoolean", name);
		jsonVal.call<void>("requiredObject", name);
		jsonVal.call<void>("optionalString", name);
		jsonVal.call<void>("optionalIntegral", name);
		jsonVal.call<void>("optionalNumber", name);
		jsonVal.call<void>("optionalBoolean", name);
		jsonVal.call<void>("optionalObject", name);
	}

	template<class BinderFn>
	void bindEntity(const std::string name, BinderFn bind)
	{
		Read reader = createObjectReader(name);
		bind(reader);
	}
			
};

}//emscripten
}//nbdl

#endif
