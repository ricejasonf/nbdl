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

//normally do this inside the MyApi scope but the tests need it
template<typename... Ts>
using Path = nbdl::CreatePath<Ts...>;

//todo ditch the wrapper
struct MyApi
{
	using Definition = nbdl::ApiDefinition<
		nbdl::AccessPoint<
			Path<Client>,
			nbdl::Actions<nbdl::actions::Read>
		>,
		nbdl::AccessPoint<
			Path<Client, MyEntity>, 
			nbdl::Actions<
				nbdl::actions::Create,
				nbdl::actions::Read,
				nbdl::actions::Update,
				nbdl::actions::Delete
			>
		>,
		nbdl::AccessPoint<
			Path<Client, MyEntity, MyEntityAttribute>, 
			nbdl::Actions<nbdl::actions::Read>
		>,
		nbdl::AccessPoint<
			Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, 
			nbdl::Actions<
				nbdl::actions::Create, 
				nbdl::actions::Read
			>
		>
	>;
};

TEST_CASE("Defined API should indicate specified AccessPoints.", "[api]") 
{
	using Def = MyApi::Definition;

	CHECK_FALSE((Def::template HasAction<Path<Client>, nbdl::actions::Create>::value));
	CHECK_FALSE((Def::template HasAction<Path<Client>, nbdl::actions::Update>::value));
	CHECK((Def::template HasAction<Path<Client>, nbdl::actions::Read>::value));

	CHECK((Def::template HasAction<Path<Client, MyEntity>, nbdl::actions::Create>::value));
	CHECK((Def::template HasAction<Path<Client, MyEntity>, nbdl::actions::Read>::value));
	CHECK((Def::template HasAction<Path<Client, MyEntity>, nbdl::actions::Update>::value));
	CHECK((Def::template HasAction<Path<Client, MyEntity>, nbdl::actions::Delete>::value));

	CHECK((Def::template HasAction<Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, nbdl::actions::Create>::value));
	CHECK((Def::template HasAction<Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, nbdl::actions::Read>::value));
	CHECK_FALSE((Def::template HasAction<Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, nbdl::actions::Update>::value));
	CHECK_FALSE((Def::template HasAction<Path<Client, MyEntity, MyEntityAttribute, AttributeStatus>, nbdl::actions::Delete>::value));
}
