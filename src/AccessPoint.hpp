#ifndef NBLD_ACCESS_POINT_HPP
#define NBLD_ACCESS_POINT_HPP

namespace nbdl {

template<typename PathType, typename ActionsType>
struct AccessPoint
{
	using Path = PathType;
	using Actions = ActionsType;
};

}//nbdl

#endif
