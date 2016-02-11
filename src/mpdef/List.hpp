//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_LIST_HPP
#define NBDL_MPDEF_LIST_HPP

#include<boost/hana.hpp>
#include<boost/hana/experimental/types.hpp>
#include<utility>

namespace mpdef {

template<typename ...T>
struct List { };

struct ListTag;

struct MakeList
{
  template<typename ...T>
  constexpr auto operator()(T...) const
  {
    return mpdef::List<T...>{};
  }
};
constexpr MakeList make_list{};

// Comparable Operators

template<typename... T, typename... U>
constexpr auto operator==(mpdef::List<T...> t, mpdef::List<U...> u)
{
  return boost::hana::equal(t, u);
}

template<typename... T, typename... U>
constexpr auto operator!=(mpdef::List<T...> t, mpdef::List<U...> u)
{
  return boost::hana::not_equal(t, u);
}

namespace detail {
  
  namespace types_detail = boost::hana::types_detail;

  template<std::size_t i, typename... T>
  struct nth_type
  {
    using Indexer = types_detail::indexer<std::make_index_sequence<sizeof...(T)>, T...>;
    using type = typename decltype(types_detail::get_elt<i>(Indexer{}))::type;
  };

}//detail


}//mpdef

namespace boost { namespace hana {

  template<typename ...T>
  struct tag_of<mpdef::List<T...>> { using type = mpdef::ListTag; };

  template<>
  struct make_impl<mpdef::ListTag>
  {
    template<typename... T>
    static constexpr auto apply(T...) 
      -> mpdef::List<T...>
    {
      return {};
    }
  };

  // Foldable

  template<>
  struct unpack_impl<mpdef::ListTag>
  {
    template<typename... T, typename F>
    static constexpr auto apply(mpdef::List<T...>, F&& f)
    {
      return std::forward<F>(f)(T{}...);
    }
  };

  // Functor

  template<>
  struct transform_impl<mpdef::ListTag>
  {
    template<typename... T, typename F>
    static constexpr auto apply(mpdef::List<T...>, F&& f)
      -> mpdef::List<decltype(f(T{}))...>
    {
      return {};
    }
  };

  // Iterable

  template<>
  struct at_impl<mpdef::ListTag>
  {
    template<typename... T, typename I>
    static constexpr auto apply(mpdef::List<T...>, I)
      -> typename mpdef::detail::nth_type<I::value, T...>::type
    {
      return {};
    }
  };

  template<>
  struct is_empty_impl<mpdef::ListTag>
  {
    template<typename... T>
    static constexpr auto apply(mpdef::List<T...>)
      -> hana::bool_<sizeof...(T) == 0>
    {
      return {};
    }
  };

  template<>
  struct drop_front_impl<mpdef::ListTag>
  {
    template<typename... T>
    static constexpr auto apply(mpdef::List<T...>)
      -> hana::bool_<sizeof...(T) == 0>
    {
      return {};
    }
  };

  // Sequence

  template<>
  struct Sequence<mpdef::ListTag>
  {
    static constexpr bool value = true;
  };

}}//boost::hana

#endif
