#ifdef NBDL_LAMBDA_TRAITS_HPP
#define NBDL_LAMBDA_TRAITS_HPP

namespace nbdl {

template<typename T>
struct LambdaTraits
{
	using ReturnType = typename LambdaTraits<decltype(&T::operator())>::ReturnType;
};
template<typename ClassType, typename Return, typename... Args>
struct LambdaTraits<Return(ClassType::*)(Args...) const>
{
	using ReturnType = Return;
};

}//nbdl

#endif
