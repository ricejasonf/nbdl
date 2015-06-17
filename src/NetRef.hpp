#ifndef NBDL_NETFREF_HPP
#define NBDL_NETFREF_HPP

namespace nbdl {

template<typename EntityType>
class NetRef
{
	using Entity = EntityType;

	Promise<Entity> read()
	{
		Promise<Entity> promise = makePromise<Entity>([keys](Promise<Entity> promise) {
			//check store
			boost::optional<Entity> entity = Store<Entity>::get(*this);
			if (entity)
				promise.resolve(*entity);
			else
			{
				Promise(
					Server::read(*this),
					[promise](Entity entity) {
						Store<Entity>::suggest(entity);
						promise.resolve(entity);
						return Promise::Done;
					},
					//Promise::Handler<Promise::Fail>([promise]() {
					Promise::FailHandler([promise]() {
						promise.reject(Promise::Fail);
					}),
					Promise::NotFoundHandler([promise]() {
						promise.reject(Promise::NotFound);
					})
				);
			}
		});
		return promise;
	}
}

}//nbdl

#endif
