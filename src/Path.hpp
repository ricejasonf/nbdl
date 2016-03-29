//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_PATH_HPP
#define NBDL_PATH_HPP

#include <nbdl/concept/Entity.hpp>

#include <utility>
#include <type_traits>
#include <boost/hana.hpp>

#include "details/Hash.hpp"

namespace nbdl {

namespace hana = boost::hana;

struct path_tag {};

template<typename Spec>
class path
{
  friend struct boost::hana::unpack_impl<nbdl::path_tag>;
  friend struct boost::hana::equal_impl<nbdl::path_tag, nbdl::path_tag>;

  static constexpr auto spec() { return Spec{}; }

  static auto parents()
  {
    return hana::unpack(spec(),
      [](auto... j) {
        return hana::make_tuple(hana::first(j)...);
      });
  }

  static auto entity_types()
  {
    constexpr auto spec_ = spec();
    return hana::unpack(
      hana::range_c<int, 0, hana::length(spec_)>,
      [&](auto... i) {
        return hana::make_map(hana::make_pair(
          hana::at(parents(), i), i)...);
      });
  }

  template<typename T>
  static constexpr auto entity_type_index(T type)
  {
    auto types = entity_types();
    return types[type];
  }

  //todo filter out empty or void
  static auto storage_type()
  {
    return hana::unpack(spec(),
      [](auto... t) {
        return hana::type_c<decltype(hana::make_tuple(
            std::declval<typename decltype(+hana::second(t))::type>()...
          ))>;
      });
  }

  using Key = typename decltype(+hana::second(hana::back(spec())))::type;
  using Storage = typename decltype(storage_type())::type;

  Storage storage;

  public:

  using hana_tag = path_tag;
  using Entity = typename decltype(+hana::first(hana::back(spec())))::type;
  using Parent = path<decltype(hana::drop_back(spec(), hana::size_c<1>))>;

  static_assert(nbdl::Entity<Entity>::value, "");

  template<typename EntityType, typename KeyType>
  static constexpr auto create_child_type(EntityType e, KeyType k)
  {
    return hana::type_c<path<decltype(hana::append(spec(), hana::make_pair(e, k)))>>;
  }

  template<typename... Args>
  path(Args... args) :
    storage(hana::make_tuple(args...))
  {}

  template<typename E = Entity>
  Key get_key() const
  {
    return hana::at(storage, entity_type_index(hana::type_c<E>));
  }

  Parent parent() const
  {
    return hana::unpack(hana::drop_back(storage, hana::size_c<1>),
      [](auto... n) {
        return Parent(n...);
      });
  }

  struct hash_fn
  {
    std::size_t operator()(const path& path) const
    {
      std::size_t seed;
      hana::for_each(path.storage,
        [&](const auto& x) {
          nbdl::details::hash_combine(seed, x);
        });
      return seed;
    }
  };

  struct pred_fn
  {
    bool operator()(const path& t1, const path& t2) const
    {
      return hana::equal(t1.storage, t2.storage);
    }
  };

};

struct make_path_type_from_pairs_fn
{
  template<typename... Pairs>
  constexpr auto operator()(Pairs... pairs) const
  {
    using Spec = decltype(hana::make_tuple(pairs...));
    return hana::type_c<path<Spec>>;
  }
};
constexpr make_path_type_from_pairs_fn make_path_type_from_pairs{};

template<typename Key, typename... EntityType>
constexpr auto path_type = make_path_type_from_pairs(
    hana::make_pair(hana::type_c<EntityType>, hana::type_c<Key>)...
  );

}//nbdl

/*
 * Specializations for Hana
 */
namespace boost { namespace hana {

template<>
struct unpack_impl<nbdl::path_tag>
{
  template<typename Xs, typename F>
  static constexpr decltype(auto) apply(Xs&& xs, F&& f)
  {
    return hana::unpack(static_cast<Xs&&>(xs).storage, static_cast<F&&>(f));
  }
};

template<>
struct equal_impl<nbdl::path_tag, nbdl::path_tag>
{
  template<typename X, typename Y>
  static constexpr decltype(auto) apply(X&& x, Y&& y)
  {
    return hana::equal(
      static_cast<X&&>(x).storage,
      static_cast<Y&&>(y).storage);
  }
};

}}//boost/hana

#endif
