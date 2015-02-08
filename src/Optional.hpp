#ifndef NBDL_OPTIONAL_HPP
#define NBDL_OPTIONAL_HPP

#include<type_traits>

namespace nbdl {

template<typename T, template<typename> class TT>
struct IsSpecializationOf : std::false_type { };

template<typename T, template<typename> class TT>
struct IsSpecializationOf<TT<T>, TT> : std::true_type { };

template<typename T, class Enable = void>
struct IsOptional : std::false_type {};

template<typename T, typename = void, class = void>
struct EnableIfOptional;
template<typename T, typename R>
struct EnableIfOptional<T, R, typename std::enable_if<IsOptional<T>::value>::type>
{
	using type = R;
};

}//nbdl

#include "optional/boost.hpp"

#endif;
