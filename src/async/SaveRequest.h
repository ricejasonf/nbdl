#ifndef SAVEREQUEST_H
#define SAVEREQUEST_H

#include<functional>

class SaveRequestBase
{
	std::unique_ptr<PathNode> pathNode;

	public:

	SaveRequestBase(std::unique_ptr<PathNode> p) :
		pathNode(p) {}
	SaveRequestBase(SaveRequestBase &&) {}
	virtual ~SaveRequestBase();

	virtual void callSuccess() = 0;
	virtual void callNotFound() = 0;
	virtual void callFail() = 0;
	virtual Entity::Ref getEntityRef() = 0;
};

template<class T>
class SaveRequest
{
	std::function<void()> _success;
	std::function<void()> _notFound;
	std::function<void()> _fail;
	T entity;

	public:

	SaveRequest(std::unique_ptr<PathNode> p) :
		SaveRequestBase(p) {}

	SaveRequest &success(std::function<void()> fn)
	{
		_success = fn;	
		return *this;
	}

	SaveRequest &notFound(std::function<void()> fn)
	{
		_notFound = fn;
		return *this;
	}

	SaveRequest &fail(std::function<void()> fn)
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
