#ifndef NBDL_OPTIONAL_BOOST_HPP
#define NBDL_OPTIONAL_BOOST_HPP

#include<boost/optional.hpp>

//recognize boost::optional as an optional value
template<typename T>
struct is_optional<T, 
	typename std::enable_if<is_specialization_of<T, boost::optional>::value>::type>
	: std::true_type {};

#endif
