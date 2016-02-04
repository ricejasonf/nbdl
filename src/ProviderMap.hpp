//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PROVIDER_MAP_HPP
#define NBDL_PROVIDER_MAP_HPP

#include<Make.hpp>

#include<boost/hana.hpp>
#include<utility>

namespace nbdl {

namespace hana = boost::hana;

namespace detail {
    // extractKeys - returns pairs of each element and itself
    struct ExtractKeys
    {
        template<typename TypesType>
        constexpr auto operator()(TypesType s) const {
            return decltype(hana::unpack(typename TypesType::type{},
                hana::make_tuple
                ^hana::on^
                hana::reverse_partial(hana::make_pair, s)
            )){};
        }
    };
    constexpr ExtractKeys extractKeys{};
}//detail

template<typename ...Pair>
struct ProviderMap
{
    // the keys must be `type<set<path...>>`
    using Storage = decltype(hana::make_map(std::declval<Pair>()...));

    // each key is a hana::set which contain the keys we
    // want to use to lookup a provider
    using Lookup = decltype(hana::unpack(
        hana::flatten(hana::unpack(hana::keys(std::declval<Storage>()),
            hana::make_tuple ^hana::on^ detail::extractKeys)),
        hana::make_map
    ));

    constexpr ProviderMap()
        : storage()
    { }

    constexpr ProviderMap(Pair&&... p)
        : storage(hana::make_map(std::forward<Pair>(p)...))
    { }

    constexpr ProviderMap(Pair const&... p)
        : storage(hana::make_map(p...))
    { }

    constexpr ProviderMap(Pair&... p)
        : storage(hana::make_map(p...))
    { }

    template<typename T>
    constexpr decltype(auto) operator[](T t) const {
        return hana::at_key(storage, hana::at_key(Lookup{}, t));
    }

    Storage storage;
};

}//nbdl

#if 0
//ended up not using this in the test
namespace boost { namespace hana {

template<typename ...Pair>
struct at_key_impl<nbdl::ProviderMap<Pair...>> {
    template<typename M, typename T>
    constexpr decltype(auto) operator()(M&& m, T&& t) {
       return std::forward<M>(m)[std::forward<T>(t)];
    }
};

}}//boost::hana
#endif

#endif
