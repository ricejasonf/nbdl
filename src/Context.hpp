//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_CONTEXT_HPP
#define NBDL_CONTEXT_HPP

namespace nbdl {

template<
	typename ProviderMap,
  typename Consumers,
	typename StoreMap>
class Context
{
  ProviderMap providers;
  Consumers consumers;
	StoreMap stores;

  // should be callable only by providers
  template<typename Message>
  auto pushDownstream(Message&&)
  {
    // access providers, consumers and stores
  }

  // should be callable only by consumers
  template<typename Message>
  auto pushUpstream(Message&&)
  {
    // access providers, consumers and stores
  }

	public:

};

}//nbdl

#endif
