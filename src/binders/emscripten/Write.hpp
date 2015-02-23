#ifndef NBDL_BINDERS_EMSCRIPTEN_WRITE_HPP
#define NBDL_BINDERS_EMSCRIPTEN_WRITE_HPP

#include<string>
#include<emscripten/val.h>

namespace nbdl {
namespace binders {
namespace emscripten {

namespace emscripten = ::emscripten;

class Write
{
	public:

	emscripten::val emVal;

	Write() :
		emVal(emscripten::val::object()) {}

	template<typename T>
	void bindMember(const std::string name, T &t)
	{
		emVal.set(name, emscripten::val(t));
	}

	template<class BinderFn>
	void bindEntity(const std::string name, BinderFn bind)
	{
		Write writer;
		bind(writer);
		emVal.set(name, writer.emVal);
	}
};

}//emscripten
}//binders
}//nbdl

#endif
