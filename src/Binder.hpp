#ifndef BINDER_HPP
#define BINDER_HPP

template<class Impl>
class Binder
{
	public:

	template<typename T>
	typename std::enable_if<!std::is_base_of<EntityBase, T>::value>::type
	void bindMember(const std::string name, T &field)
	{
		static_cast<Impl*>(this)->bind(name, field);
	}

	template<typename T>
	typename std::enable_if<!std::is_base_of<EntityBase, T>::value>::type
		bindMember(const std::string name, T &field)
	{
		static_cast<Impl*>(this)->bindEntity(name, field);
	}
};

#endif
