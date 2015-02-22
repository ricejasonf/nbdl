#ifndef NBDL_EMSCRIPTEN_READ_HPP
#define NBDL_EMSCRIPTEN_READ_HPP

#include<type_traits>
#include<string>
#include<emscripten/val.h>

namespace nbdl {
namespace emscripten {

class Read
{
	//entities will be wrapped by javascript
	//side code to support duck typing of properties
	//to fail on invalid inputs
	emscripten::val &emVal;

	Read createObjectReader(const std::string name)
	{
		Read reader(emVal[name]);
		return reader;
	}

	public:

	Read(emscripten::val &val) :
		emVal(val) {}

	template<typename T>
	std::enable_if<std::is_integral<T>::value>::type
		bindMember(const std::string name, T &member)
	{
		emVal.call<void>("requiredIntegral", name);
		member = emVal[name];
	}
	//todo double, float, bool, optionals

	template<>
	void bindMember<std::string>(const std::string name, std::string &member)
	{
		emVal.call<void>("requiredString", name);
		member = emVal[name];
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
/*
	//ideas
	emVal.call<void>("requiredIntegral", name);
	emVal.call<void>("requiredString", name);
	emVal.call<void>("requiredNumber", name);
	emVal.call<void>("requiredBoolean", name);
	emVal.call<void>("requiredObject", name);
	emVal.call<void>("optionalString", name);
	emVal.call<void>("optionalIntegral", name);
	emVal.call<void>("optionalNumber", name);
	emVal.call<void>("optionalBoolean", name);
	emVal.call<void>("optionalObject", name);
*/

#endif
