#ifndef RETRIEVEREQUEST_H
#define RETRIEVEREQUEST_H

#include<functional>

class RetrieveRequestBase
{
	public:

	virtual ~RetrieveRequestBase();

	virtual void callSuccess() = 0;
	virtual void callNotFound() = 0;
	virtual void callFail() = 0;
	virtual Entity::Ref getEntityRef() = 0;
};

template<class T>
class RetrieveRequest
{
	std::function<void(T)> _success;
	std::function<void()> _notFound;
	std::function<void()> _fail;
	T entity;

	public:

	RetrieveRequest(std::unique_ptr<PathNode> p) :
		RetrieveRequestBase(p) {}

	RetrieveRequest &success(std::function<void, T> fn)
	{
		_success = fn;	
		return *this;
	}

	RetrieveRequest &notFound(std::function<void()> fn)
	{
		_notFound = fn;
		return *this;
	}

	RetrieveRequest &fail(std::function<void()> fn)
	{
		_fail = fn;
		return *this;
	}

	void callSuccess()
	{
		//client must populate entity
		if (_callSuccess)
			_success(entity);
	}

	void callNotFound() 
	{
		if (_notFound)
			_notFound();
		else
			_fail();
	}

	void callFail() 
	{
		if (_fail())
			_fail();
	}

	T::Ref getEntityRef()
	{
		return Entity::Ref(entity);
	}
};

#endif
