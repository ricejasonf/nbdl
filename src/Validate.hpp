#ifndef NBDL_VALIDATE_HPP
#define NBDL_VALIDATE_HPP

namespace nbdl {

namespace detail {

template<typename Errors, typename M, typename T>
void validateString(Errors &e, T& member) {}
template<class T, class Traits, class Alloc>
void validateString(Errors &e, basic_string<T, Traits, Alloc> &member)
{
	if (!validateStringMaxLength(errors, member))
		return;
}
		
		



struct ValidationBinder
{
	struct GnosticBinder;
	struct Validate<M>

	template<typename M>
	void bindMember(typename M::OwnerType &entity)
	{
		M::MemberType &m;
		if (
	}
	template<typename M>
	void bindEntity(typename M::OwnerType &entity)
	{

	}
};

}//detail

template<class ValidationBinder, class Entity>
void validate(ValidationBinder &errors, Entity &entity)
{
	ValidateEntity	
}

}//nbdl
#endif
