#ifndef NBDL_EMSCRIPTEN_WRITE_HPP
#define NBDL_EMSCRIPTEN_WRITE_HPP

#include<string>
#include<emscripten/val.h>

namespace nbdl {
namespace emscripten {

class Write
{
	emscripten::val jsonVal;

	public:

	Write() :
		jsonVal(emscripten::val::object()) {}

	template<typename T>
	void bindMember(const std::string name, T &t)
	{
		jsvonVal.set(name, emscripten::val(t))
	}

	template<class BinderFn>
	void bindEntity(const std::string name, BinderFn bind)
	{
		Write writer();
		bind(writer);
		jsonVal.set(name, writer.jsonVal);
	}
};

}//emscripten
}//nbdl

#endif
