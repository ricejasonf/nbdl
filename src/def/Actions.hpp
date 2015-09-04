//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DEF_ACTIONS_HPP
#define NBDL_DEF_ACTIONS_HPP

#include<type_traits>

namespace nbdl_def {

struct Create {};
struct Read {};
struct Update {};
struct Delete {};

template<typename... Ts>
class Actions
{
	public:

	template<typename>
	using HasAction = std::false_type;
};

template<typename T, typename... Ts>
class Actions<T, Ts...>
{
	template<typename ActionType, class = void>
	struct HasAction_
	{
		using Next = typename Actions<Ts...>::template HasAction<ActionType>;
		static constexpr bool value = Next::value;
	};
	template<typename ActionType>
	struct HasAction_<ActionType,
		typename std::enable_if<std::is_same<T, ActionType>::value>::type>
	{
		static constexpr bool value = true;
	};

	//restrict to valid actions
	static constexpr bool hasAction(Create) { return HasAction_<Create>::value; }
	static constexpr bool hasAction(Read) { return HasAction_<Read>::value; }
	static constexpr bool hasAction(Update) { return HasAction_<Update>::value; }
	static constexpr bool hasAction(Delete) { return HasAction_<Delete>::value; }

	public:

	template<typename ActionType>
	struct HasAction 
	{
		static constexpr bool value = hasAction(ActionType{});
	};
};

}//nbdl_def

#endif
