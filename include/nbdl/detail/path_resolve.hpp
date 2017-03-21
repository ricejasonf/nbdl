//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_RESOLVE_PATH_HPP
#define NBDL_DETAIL_RESOLVE_PATH_HPP

#include <nbdl/path.hpp>

#include <boost/hana/concept/constant.hpp>

namespace nbdl::detail
{
  namespace hana = boost::hana;

  template <typename T>
  struct path_node_type
  {
    using type = T;
  };

  template <typename Key, typename T>
  struct path_node_type<nbdl::path_node<Key, T>>
  {
    using type = Key;
  };

  struct make_path_storage_tuple_fn
  {
    template <typename ...PathNodes>
    constexpr auto operator()(nbdl::path<PathNodes...>)
    {
      return hana::tuple<
        typename decltype(path_node_key<PathNodes>)::type...
      >{};
    }
  };

  constexpr make_path_storage_tuple_fn make_path_storage_tuple{};

  template <typename ...PathNodes, typename S, typename ResolveFn, typename RejectFn>
  void resolve_path(nbdl::path<PathNodes...>
                  , S const& store
                  , ResolveFn&& resolve
                  , RejectFn&& reject);


  struct resolve_path_node_fn
  {
    template <typename Node, typename S, typename ResolveFn, typename RejectFn>
    constexpr void operator()(Node node
                            , S const& /* store */
                            , ResolveFn&& resolve
                            , RejectFn&& /* reject */)
    {
      if constexpr(hana::is_a<nbdl::path_node_tag, Node>)
      {
        /*
         * TODO Recursively match children of Path
         */
        static_assert(
          std::is_same<Path, void>::value
        , "Nbdl does not currently support nested paths"
        );

        static_assert(
          hana::is_a<nbdl::path_tag, Path>
        , "nbdl::path_node value must be a nbdl::path"
        );
      }
      else
      {
        std::forward<ResolveFn>(resolve)(node);
      }
    }
  };

  constexpr resolve_path_node_fn resolve_path_node{};

  struct assign_path_node_fn
  {
    template <typename Storage, typename I>
    constexpr auto operator()(Storage& storage, I i, Value&& value) const
    {
      storage[i] = std::forward<Value>(value);
    }
  };

  constexpr auto assign_path_node = hana::curry<3>(assign_path_node_fn{});

  template <typename ...PathNodes, typename S, typename ResolveFn, typename RejectFn>
  void resolve_path(nbdl::path<PathNodes...>
                  , S const& store
                  , ResolveFn&& resolve
                  , RejectFn&& reject)
  {
    // TODO modify value of storage tuple for each element
    // A fold similar to nbdl::pipe will be needed to
    // handle rejections
    constexpr auto nodes = hana::make_tuple(PathNodes{}...);

    auto handlers = hana::unpack(
      hana::make_range(hana::size_c<0>, hana::length(nodes))
    , hana::on(
        hana::make_tuple
      , assign_path_node(mpdef::list<PathNodes...>, storage)
      )
    );

    auto storage = make_path_storage_tuple();
    hana::fold_left(
      handlers
    , [&](auto&& ) { std::forward<ResolveFn>(resolve)(storage); }
    , [&](auto state, auto x)
      {
        // "results" should be empty or a rejection tag
        return [resolve = detail::make_resolver(state), &x](auto&& ...results)
        {
          if constexpr(is_rejection<std::decay_t<decltype(results)>...>::value)
          {
            // catch it and be done
            nbdl::catch_(std::forward<RejectFn>(reject))(
              std::forward<decltype(results)>(results)...
            );
          }
          else
          {
            resolve_path_node(
              
            );

          }
        }
      }
    )();
  }
}

#endif
