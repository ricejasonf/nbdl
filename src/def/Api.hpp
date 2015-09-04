//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_API_DEFINITION_HPP
#define NBDL_DEF_API_DEFINITION_HPP

#include<type_traits>
#include "def/AccessPoint.hpp"

namespace nbdl_def {

template<typename... Ts>
class Api
{
	public:

	template<typename, typename>
	using HasAction = std::false_type;

	template<int index, int current_index = 0>
	using GetPath = void;
	static constexpr std::size_t size = 0;
};

template<typename T, typename... Ts>
class Api<T, Ts...>
{
	template<typename PathType, typename ActionType, class = void>
	struct HasAction_
	{
		using Next = typename Api<Ts...>::template HasAction<PathType, ActionType>;
		static constexpr bool value = Next::value;
	};
	template<typename PathType, typename ActionType>
	struct HasAction_<PathType, ActionType,
		typename std::enable_if<std::is_same<typename T::Path, PathType>::value>::type>
	{
		static constexpr bool value = T::Actions::template HasAction<ActionType>::value;
	};

	template<int index, int current_index, class = void>
	struct GetPath_
	{
		using Next = typename Api<Ts...>::template GetPath<index, (current_index + 1)>;
		using Type = typename Next::Type;
	};
	template<int index, int current_index> 
	struct GetPath_<index, current_index, 
		typename std::enable_if<(index == current_index)>::type>
	{
		using Type = typename T::Path;
	};

	public:

	template<typename PathType, typename ActionType>
	struct HasAction
	{
		static constexpr bool value = HasAction_<PathType, ActionType>::value;
	};

	template<int index, int current_index = 0>
	struct GetPath
	{
		using Type = typename GetPath_<index, current_index>::Type;
	};
	static constexpr std::size_t size = 1 + Api<Ts...>::size;
};

template<typename... Us, typename... Ts>
class Api<Api<Us...>, Ts...>
{
	public:

	template<typename PathType, typename ActionType>
	using HasAction = typename Api<Us...>::template HasAction<PathType, ActionType>::value;

	template<int index, int current_index>
	using GetPath = typename Api<Us...>::template GetPath<index, current_index>::Type;
	static constexpr std::size_t size = Api<Us...>::size + Api<Ts...>::size;
};

}//nbdl_def

#endif
