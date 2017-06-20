//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_PARAMS_VIEW_HPP
#define NBDL_DETAIL_PARAMS_VIEW_HPP

#include <array>
#include <boost/hana/ext/std/array.hpp>
#include <boost/hana/filter.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/type.hpp>
#include <functional>
#include <mpdef/list.hpp>
#include <type_traits>
#include <utility>

namespace nbdl::detail
{
  namespace hana = boost::hana;

  template <typename T>
  struct is_reference_wrapper
    : std::false_type
  { };

  template <typename T>
  struct is_reference_wrapper<std::reference_wrapper<T>>
    : std::true_type
  { };

  // pos is the index of the element in the storage array
  template <std::size_t pos, typename T>
  struct ref_placeholder
  { };


  template <typename T>
  struct is_ref_placeholder
    : std::false_type
  { };

  template <std::size_t pos, typename T>
  struct is_ref_placeholder<ref_placeholder<pos, T>>
    : std::true_type
  { };

  template <std::size_t offset>
  struct offset_ref_placholder
  {
    template <std::size_t pos, typename T>
    constexpr auto operator()(ref_placeholder<pos, T>)
      -> ref_placeholder<(pos + offset), T>
    { return {}; }
  };

  template <typename T>
  struct params_view_hydrate_value
  {
    template <typename ParamsView>
    constexpr auto operator()(ParamsView const&) const
    {
      return T{}; 
    }
  };

  template <std::size_t pos, typename T>
  struct params_view_hydrate_value<ref_placeholder<pos, T>>
  {
    template <typename ParamsView>
    constexpr auto operator()(ParamsView const& p) const
    {
      return std::cref(*static_cast<T const*>(
        hana::at_c<pos>(p.storage))
      );
    }
  };

  struct params_view_tag { };

  /*
   * params_view - A tuple optimized to store reference_wrappers and stateless values
   */
  template <std::size_t storage_size, typename List>
  struct params_view
  {
    using Storage = std::array<void const*, storage_size>;
    Storage storage;

    params_view(Storage s)
      :storage(std::move(s))
    { }
  };

  template <typename ...Xs>
  struct params_view<0, mpdef::list<Xs...>> { };

  struct params_view_get_storage_impl
  {
    template <typename ParamsView>
    auto operator()(ParamsView&& p) const
      -> decltype(std::forward<ParamsView>(p).storage)
    {
      return std::forward<ParamsView>(p).storage;
    }
  };

  // returns Foldable to unpack
  constexpr auto params_view_get_storage = [](auto&& p)
  {
    return hana::sfinae(params_view_get_storage_impl{})(std::forward<decltype(p)>(p))
      .value_or(mpdef::list<>{});
  };

  /*
   * make_params_view - Only makes an empty params_view to keep things simple
   */
  struct make_params_view_fn
  {
    constexpr auto operator()() const
    {
      return params_view<0, mpdef::list<>>{};
    }
  };

  constexpr make_params_view_fn make_params_view{};

  /*
   * params_view_append - Appends a stateless value or reference_wrapper
   *                      to a params_view
   */
  struct params_view_append_fn
  {
    template <std::size_t storage_size, typename X, typename ...Xs>
    constexpr auto operator()(X x
                            , params_view<storage_size, mpdef::list<Xs...>> const& p) const
    {
      if constexpr(storage_size > 0)
      {
        if constexpr(is_reference_wrapper<X>::value)
        {
          return hana::unpack(p.storage, [&](auto* ...ptr)
          {
            using NewX = ref_placeholder<storage_size, typename X::type>;
            return params_view<storage_size + 1, mpdef::list<Xs..., NewX>>(
              std::array<void const*, storage_size + 1>{{
                ptr...
              , reinterpret_cast<void const*>(&(x.get()))
              }}
            );
          });
        }
        else
        {
          return params_view<storage_size, mpdef::list<Xs..., X>>(p.storage);
        }
      }
      else // has no run-time state yet
      {
        if constexpr(is_reference_wrapper<X>::value)
        {
          using NewX = ref_placeholder<0, typename X::type>;
          return params_view<1, mpdef::list<Xs..., NewX>>(
            std::array<void const*, 1>{{reinterpret_cast<void const*>(&(x.get()))}}
          );
        }
        else
        {
          return params_view<storage_size, mpdef::list<Xs..., X>>{};
        }
      }
    }
  };
  
  constexpr params_view_append_fn params_view_append{};

  struct params_view_concat_fn
  {
    template <std::size_t s_1, std::size_t s_2, typename ...Xs, typename ...Ys>
    constexpr auto operator()(params_view<s_1, mpdef::list<Xs...>> const& p1
                            , params_view<s_2, mpdef::list<Ys...>> const& p2) const
    {
      return hana::unpack(params_view_get_storage(p1), [&](auto ...storage_xs)
      {
        return hana::unpack(params_view_get_storage(p2), [&](auto ...storage_ys)
        {
          constexpr auto new_ys = hana::transform(mpdef::list<Ys...>{}, [](auto y)
          {
            if constexpr(is_ref_placeholder<decltype(y)>::value)
              return offset_ref_placholder<s_1>{}(y);
            else
              return y;
          });
          return hana::unpack(new_ys, [&](auto ...new_y)
          {
            if constexpr(s_1 + s_2 > 0)
            {
              return params_view<(s_1 + s_2), mpdef::list<Xs..., decltype(new_y)...>>(
                std::array<void const*, (s_1 + s_2)>{{storage_xs..., storage_ys...}}
              );
            }
            else
            {
              return params_view<0, mpdef::list<Xs..., decltype(new_y)...>>{};
            }
          });
        });
      });
    }
  };

  constexpr params_view_concat_fn params_view_concat{};
}

namespace boost::hana
{
  template <std::size_t storage_size, typename List>
  struct tag_of<nbdl::detail::params_view<storage_size, List>>
  { using type = nbdl::detail::params_view_tag; };

  // Foldable

  template <>
  struct unpack_impl<nbdl::detail::params_view_tag>
  {
    template <std::size_t storage_size, typename ...Xs, typename F>
    static constexpr auto apply(nbdl::detail::params_view<storage_size, mpdef::list<Xs...>> const& p
                              , F&& f)
    {
      return std::forward<F>(f)(nbdl::detail::params_view_hydrate_value<Xs>{}(p)...);
    } 
  };
}

#endif
