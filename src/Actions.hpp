#ifndef #NBDL_ACTIONS_HPP
#define #NBDL_ACTIONS_HPP

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
		using Next = Actions<Ts...>::template HasAction<ActionType>;
		static constexpr bool value = Next::value;
	};
	template<typename ActionType>
	struct HasAction_<ActionType,
		typename std::enable_if<std::is_same<T, ActionType>::value>::type>
	{
		static constexpr bool value = true;
	};

	public:

	//restrict to valid actions
	template<typename ActionType>
	struct HasAction {};
	template<> struct HasAction<actions::Create> { static constexpr bool value = HasAction_<actions::Create>::value; };
	template<> struct HasAction<actions::Read> { static constexpr bool value = HasAction_<actions::Read>::value; };
	template<> struct HasAction<actions::Update> { static constexpr bool value = HasAction_<actions::Update>::value; };
	template<> struct HasAction<actions::Delete> { static constexpr bool value = HasAction_<actions::Delete>::value; };
};


}//nbdl

#endif
