//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<nbdl>
#include "catch.hpp"

struct Client
{
	int id;
};
struct MyEntity
{
	int id;
	int client_id;
};
struct MyEntityAttribute
{
	int id;
	int my_entity_id;
};
struct AttributeStatus
{
	int id;
	int attribute_id;
};
namespace nbdl {
	NBDL_ENTITY(
		Client,
			id);
	NBDL_ENTITY(
		MyEntity,
			id,
			client_id );
	NBDL_ENTITY(
		MyEntityAttribute,
			id,
			my_entity_id );
	NBDL_ENTITY(
		AttributeStatus,
			id,
			attribute_id );
}//nbdl

namespace my_api
{
	using namespace nbdl_def;

	using Definition = Api<
		AccessPoint<
			Path<Client>,
			Actions<Read>
		>,
		AccessPoint<
			Path<Client, MyEntity>, 
			Actions<
				Create,
				Read,
				Update,
				Delete
			>
		>,
		AccessPoint<
			Path<Client, MyEntity, MyEntityAttribute>, 
			Actions<Read>
		>,
		AccessPoint<
			Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, 
			Actions<
				Create, 
				Read
			>
		>
	>;
};

template<typename... Ts>
using Path = nbdl_def::Path<Ts...>;

TEST_CASE("Defined API should indicate specified AccessPoints.", "[api]") 
{
	using Def = my_api::Definition;
	using Create =  nbdl_def::Create;
	using Read =  nbdl_def::Read;
	using Update =  nbdl_def::Update;
	using Delete =  nbdl_def::Delete;

	CHECK_FALSE((Def::template HasAction<Path<Client>, Create>::value));
	CHECK_FALSE((Def::template HasAction<Path<Client>, Update>::value));
	CHECK((Def::template HasAction<Path<Client>, Read>::value));

	CHECK((Def::template HasAction<Path<Client, MyEntity>, Create>::value));
	CHECK((Def::template HasAction<Path<Client, MyEntity>, Read>::value));
	CHECK((Def::template HasAction<Path<Client, MyEntity>, Update>::value));
	CHECK((Def::template HasAction<Path<Client, MyEntity>, Delete>::value));

	CHECK((Def::template HasAction<Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, Create>::value));
	CHECK((Def::template HasAction<Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, Read>::value));
	CHECK_FALSE((Def::template HasAction<Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, Update>::value));
	CHECK_FALSE((Def::template HasAction<Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, Delete>::value));
}