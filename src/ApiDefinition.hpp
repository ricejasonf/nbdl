#ifndef NBDL_API_DEFINITION_HPP
#define NBDL_API_DEFINITION_HPP

#include "AccessPoint.hpp"

namespace nbdl {

template<typename... Ts>
class ApiDefinition
{
	public:

	template<typename, typename>
	using HasAction = std::false_type;
};

template<typename T, typename... Ts>
class ApiDefinition<T, Ts...>
{
	template<typename PathType, typename ActionType, class = void>
	struct HasAction_
	{
		using Next = typename ApiDefinition<Ts...>::template HasAction<PathType, ActionType>;
		static constexpr bool value = Next::value;
	};
	template<typename PathType, typename ActionType>
	struct HasAction_<PathType, ActionType,
		typename std::enable_if<(
			std::is_same<typename T::Path, PathType>::value
			&& std::is_same<typename T::Actions, ActionType>::value
		)>::type>
	{
		static constexpr bool value = T::Actions::template HasAction<ActionType>::value;
	};

	public:

	template<typename PathType, typename ActionType>
	struct HasAction
	{
		static constexpr bool value = HasAction_<PathType, ActionType>::value;
	};
};

template<typename... Us, typename... Ts>
class ApiDefinition<ApiDefinition<Us...>, Ts...>
{
	public:

	template<typename PathType, typename ActionType>
	using HasAction = typename ApiDefinition<Us...>::template HasValue<PathType, ActionType>::value;
};

}//nbdl

#endif
