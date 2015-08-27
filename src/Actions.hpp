//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_ACTIONS_HPP
#define NBDL_ACTIONS_HPP

namespace nbdl {

namespace actions {

struct Create {};
struct Read {};
struct Update {};
struct Delete {};

}//actions

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
	static constexpr bool hasAction(actions::Create) { return HasAction_<actions::Create>::value; }
	static constexpr bool hasAction(actions::Read) { return HasAction_<actions::Read>::value; }
	static constexpr bool hasAction(actions::Update) { return HasAction_<actions::Update>::value; }
	static constexpr bool hasAction(actions::Delete) { return HasAction_<actions::Delete>::value; }

	public:

	template<typename ActionType>
	struct HasAction 
	{
		static constexpr bool value = hasAction(ActionType{});
	};
};
/*
template<typename T, typename... Ts> struct Actions<T, Ts...>::template HasAction<actions::Create> { static constexpr bool value = HasAction_<actions::Create>::value; };
template<typename T, typename... Ts> struct Actions<T, Ts...>::template HasAction<actions::Read> { static constexpr bool value = HasAction_<actions::Read>::value; };
template<typename T, typename... Ts> struct Actions<T, Ts...>::template HasAction<actions::Update> { static constexpr bool value = HasAction_<actions::Update>::value; };
template<typename T, typename... Ts> struct Actions<T, Ts...>::template HasAction<actions::Delete> { static constexpr bool value = HasAction_<actions::Delete>::value; };
*/


}//nbdl

#endif
