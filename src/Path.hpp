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

namespace detail
{
  struct first_or_fn
  {
    template <typename Xs, typename Default>
    constexpr decltype(auto) operator()(Xs&& xs, Default&& d)
    {
      if constexpr(decltype(hana::length(xs)){} == hana::size_c<0>)
      {
        return std::forward<Default>(d);
      }
      else
      {
        return hana::at_c<0>(std::forward<Xs>(xs));
      }
    }
  };
}

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
        return hana::type_c<decltype(hana::transform(
          hana::remove_if(
            hana::make_tuple(hana::second(t)...),
            hana::traits::is_empty
          ),
          hana::traits::declval
        ))>;
      });
  }

  using key = typename decltype(+hana::second(hana::back(spec())))::type;
  using Storage = typename decltype(storage_type())::type;
  using First = std::decay_t<decltype(detail::first_or_fn{}(
      std::declval<Storage>(),
      hana::type_c<void>
    ))>;

  Storage storage;

  public:

  using hana_tag = path_tag;
  using canonical_key = typename decltype(
    hana::if_(hana::is_a<hana::type_tag, key>, key{}, hana::type_c<key>)
  )::type;
  using entity = typename decltype(+hana::first(hana::back(spec())))::type;
  using Parent = path<decltype(hana::drop_back(spec(), hana::size_c<1>))>;
  using Entity = entity; // deprecated

  template<typename EntityType, typename KeyType>
  static constexpr auto create_child_type(EntityType e, KeyType k)
  {
    return hana::type_c<path<decltype(hana::append(spec(), hana::make_pair(e, k)))>>;
  }

  template<typename... Args>
  explicit path(First const& first, Args&&... args) :
    storage(first, std::forward<Args>(args)...)
  { }

  template<typename... Args>
  explicit path(First&& first, Args&&... args) :
    storage(std::move(first), std::forward<Args>(args)...)
  { }

  path() = default;

  template<typename E = Entity>
  key get_key() const
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

  // Generic function to convert path to not actually store the
  // yet to be assigned key. (idempotent)
  constexpr static auto make_create_path_type()
  {
    using CreatePathSpec = decltype(hana::append(
      hana::drop_back(Spec{}, hana::size_c<1>),
      hana::make_pair(
        hana::type_c<Entity>,
        hana::type_c<hana::type<canonical_key>>
      )
    ));
    return hana::type_c<nbdl::path<CreatePathSpec>>;
  }

  // Generic function to convert path to not actually store the
  // yet to be assigned key. (idempotent)
  template <typename ...Args>
  static auto make_create_path(Args&& ...args)
  {
    using CreatePath = typename decltype(make_create_path_type())::type;
    static_assert(std::is_constructible<CreatePath, Args...>{},
      "CreatePath is not constructible.");
    return CreatePath(std::forward<Args>(args)...);
  }

  constexpr static auto make_canonical_path_type()
  {
    if constexpr(hana::is_a<hana::type_tag, key>)
    {
      using CanonicalKey = typename key::type;
      using CanonicalPathSpec = decltype(hana::append(
        hana::drop_back(Spec{}, hana::size_c<1>),
        hana::make_pair(
          hana::type_c<Entity>,
          hana::type_c<CanonicalKey>
        )
      ));
      return hana::type_c<nbdl::path<CanonicalPathSpec>>;
    }
    else
    {
      return hana::type_c<nbdl::path<Spec>>;
    }
  }

  using canonical_path = typename decltype(make_canonical_path_type())::type;

  template <typename Key>
  auto canonicalize_path(Key const& k) const
  {
    return hana::unpack(
      hana::drop_back(storage, hana::size_c<1>),
      [&](auto&& ...xs)
      {
        return canonical_path(std::forward<decltype(xs)>(xs)..., k);
      }
    );
  }

  struct hash_fn
  {
    std::size_t operator()(const path& path) const
    {
      std::size_t digest = 0;
      hana::for_each(path.storage,
        [&](const auto& x) {
          nbdl::details::hash_combine(digest, x);
        });
      return digest;
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

template <>
struct unpack_impl<nbdl::path_tag>
{
  template<typename Xs, typename F>
  static constexpr decltype(auto) apply(Xs&& xs, F&& f)
  {
    return hana::unpack(static_cast<Xs&&>(xs).storage, static_cast<F&&>(f));
  }
};

template <>
struct equal_impl<nbdl::path_tag, nbdl::path_tag>
{
  template <typename X, typename Y, typename = std::enable_if_t<std::is_same<
    std::decay_t<X>,
    std::decay_t<Y>
    >::value>
  >
  static constexpr auto apply(X&& x, Y&& y)
  {
    return hana::equal(
      std::forward<X>(x).storage,
      std::forward<Y>(y).storage
    );
  }
};

}}//boost/hana

#endif
