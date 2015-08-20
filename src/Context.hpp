#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

#include "StoreCollection.hpp"

namespace nbdl {

template<typename Server, typename ApiDef>
class Context
{
	Server server;
	StoreCollection<ApiDef> store;
	
	template<typename PathType>
	using VariantType = typename StoreCollection<ApiDef>::VariantType<PathType>;

	public:

	template<typename Path, typename... MatchFns>
	void read(Path path, MatchFns... fns)
	{
		store.get(
			[&](const Path path) {
				//request from server
				server.read(path, [&](VariantType<Path> value) {
					store.suggestAssign(path, value);
				});
			},
			path, fns...);
	}
};

}//nbdl

#endif
