#ifndef BINDER_HPP
#define BINDER_HPP

namespace nbdl {

template<class Impl>
class Binder
{
	public:

	template<typename T>
	void bindMember(const std::string name, T &field)
	{
		static_cast<Impl*>(this)->bind(name, field);
	}
};

}//nbdl

#endif
