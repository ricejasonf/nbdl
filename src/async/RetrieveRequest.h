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
};

template<class Entity>
class RetrieveRequest
{
	RequestRetrieveSuccess<Entity> success_;
	RequestResourceNotFound notFound_;
	RequestFail fail_;

	Entity entity;

	public:

	RetrieveRequest(std::unique_ptr<PathNode> p) :
		RetrieveRequestBase(p) {}

	template<class Fn>
	RetrieveRequest &success(Fn fn)
	{
		success_ = fn;	
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
