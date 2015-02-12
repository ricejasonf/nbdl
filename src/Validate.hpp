#ifndef NBDL_VALIDATE_HPP
#define NBDL_VALIDATE_HPP

#include "validate/validateString.hpp"

namespace nbdl {

namespace detail {

template<typename ErrorBinder>
class ValidationBinder
{
	ErrorBinder &errors;

	public:

	struct GnosticBinder;

	ValidationBinder(ErrorBinder &errors)
		: errors(errors) {}

	template<typename M>
	void bindMember(typename M::OwnerType &entity)
	{
		M::MemberType &m = entity.*M::ptr;
		validateString<M>(errors, m);
		//todo do other validation stuffs
	}
	template<typename M>
	void bindEntity(typename M::OwnerType &entity)
	{
		ErrorBinder child = errors.createChild<M>();
		validate(child, entity.*M::ptr);
	}
};

}//detail

namespace validate { 

template<typename M, typename Errors, typename T>
void validateString(Errors &e, T& member) {}

}//validate

template<class ErrorBinder, class Entity>
void validate(ErrorBinder &errorBinder, Entity &entity)
{
	ValidationBinder<ErrorBinder> vBinder(errorBinder);
	bind(vBinder, entity);
}

}//nbdl
#endif
