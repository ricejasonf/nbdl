//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<boost/optional.hpp>
#include<type_traits>
#include<array>

template<typename T, template<typename> class TT>
struct is_specialization_of : std::false_type { };

template<typename T, template<typename> class TT>
struct is_specialization_of<TT<T>, TT> : std::true_type { };

template<typename T, class Enable = void>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<T, 
	typename std::enable_if<is_specialization_of<T, boost::optional>::value>::type>
	: std::true_type {};

template<typename T, typename = void, class = void>
struct enable_if_optional;
template<typename T, typename R>
struct enable_if_optional<T, R, typename std::enable_if<is_optional<T>::value>::type>
{
	using type = R;
};

template<typename T, class = void>
struct optional_printer
{
	static void print() { std::cout << "not optional"; }
};
template<typename T>
struct optional_printer<T, typename enable_if_optional<T>::type>
{
	static void print() { std::cout << "optional"; }
};

int main()
{
	std::cout << std::boolalpha;
	std::cout << is_optional<boost::optional<int>>::value;
	std::cout << std::endl;
	std::cout << is_optional<int>::value;
	std::cout << std::endl;
	std::cout << is_optional<std::array<int, 5>>::value;
	std::cout << std::endl;
	optional_printer<boost::optional<int>>::print();
	std::cout << std::endl;
	optional_printer<int>::print();
	std::cout << std::endl;
}
