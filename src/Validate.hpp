#ifndef NBDL_VALIDATE_HPP
#define NBDL_VALIDATE_HPP

#include "ErrorToken.hpp"

namespace nbdl {

namespace validation { 

template<typename M, typename T, typename AddError>
void validateString(T&, AddError) {}

}//validation

#include "validate/validateString.hpp"

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
		validateMember<M>(entity, [&](ErrorToken token) {
			errors.addError(MemberName<typename ErrorBinder::NameFormat, M>::value, token);	
		});
	}
	template<typename M>
	void bindEntity(typename M::OwnerType &entity)
	{
		ErrorBinder child = errors.createChild("moo");
		validate(child, entity.*M::ptr);
	}
};

}//detail

template<typename M, class AddError>
void validateMember(typename M::OwnerType &entity, AddError addError)
{
	typename M::MemberType &m = entity.*M::ptr;
	validation::validateString<M>(m, addError);
	//todo do other validation stuffs
}

template<class ErrorBinder, class Entity>
void validate(ErrorBinder &errorBinder, Entity &entity)
{
	detail::ValidationBinder<ErrorBinder> vBinder(errorBinder);
	bind(vBinder, entity);
}

}//nbdl
#endif
