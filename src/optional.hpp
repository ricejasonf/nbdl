#ifndef NBDL_OPTIONAL_HPP
#define NBDL_OPTIONAL_HPP

#include<type_traits>
#include "optional/boost.hpp"

namespace nbdl {

template<typename T, template<typename> class TT>
struct is_specialization_of : std::false_type { };

template<typename T, template<typename> class TT>
struct is_specialization_of<TT<T>, TT> : std::true_type { };

template<typename T, class Enable = void>
struct is_optional : std::false_type {};

template<typename T, typename = void, class = void>
struct enable_if_optional;
template<typename T, typename R>
struct enable_if_optional<T, R, typename std::enable_if<is_optional<T>::value>::type>
{
	using type = R;
};

template<typename T>
class optional
{
	//todo naive implementation of optional
}

template<typename T>
struct is_optional<T, 
	typename std::enable_if<is_specialization_of<T, optional>::value>::type>
	: std::true_type {};

}//nbdl
#endif;
