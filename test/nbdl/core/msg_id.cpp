//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include<nbdl>
#include<unordered_set> //for unique check
#include<catch.hpp>

TEST_CASE("Create 50000 message ids that are unique.", "[api]") 
{
	using MsgId = nbdl::MsgId;
	using Set = std::unordered_set<MsgId>;

	MsgId::RandomGenerator gen;
	Set set;
	const unsigned max_count = 5;

	for (unsigned i = 0; i < max_count; i++)
	{
		set.emplace(MsgId(gen));
	}
	REQUIRE(set.size() == max_count);
}
