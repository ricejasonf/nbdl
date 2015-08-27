//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_VALIDATE_HPP
#define NBDL_VALIDATE_HPP

#include "ErrorToken.hpp"

namespace nbdl {

namespace validation { 

template<typename M, typename T, typename AddError>
struct ValidateString
{
	static void call(T&, AddError&) {}
};

template<typename M>
struct ValidateMember
{
	template<class AddError>
	static void call(typename M::OwnerType &entity, AddError addError)
	{
		typename M::MemberType &m = entity.*M::ptr;
		validation::ValidateString<M, typename M::MemberType, AddError>
			::call(m, addError);
		//todo do other validation stuffs
		MemberCustomValidator<M>::validate(entity.*M::ptr, addError);
	}
};


}//validation

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
		validation::ValidateMember<M>::call(entity, [&](ErrorToken token) {
			errors.addError(MemberName<typename ErrorBinder::NameFormat, M>::value, token);	
		});
	}
	template<typename M>
	void bindEntity(typename M::OwnerType &owner)
	{
		errors.validateChild(MemberName<typename ErrorBinder::NameFormat, M>::value, 
			[&owner](ErrorBinder& childErrors) {
				detail::ValidationBinder<ErrorBinder> vBinder(childErrors);
				bind(vBinder, owner.*M::ptr);
			});
	}
};

}//detail

template<class ErrorBinder, class Entity>
void validate(ErrorBinder &errorBinder, Entity &entity)
{
	detail::ValidationBinder<ErrorBinder> vBinder(errorBinder);
	bind(vBinder, entity);
}

}//nbdl

#include "validate/StdBasicString.hpp"

#endif
