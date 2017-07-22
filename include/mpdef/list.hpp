//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_MPDEF_LIST_HPP
#define NBDL_MPDEF_LIST_HPP

#include <boost/hana.hpp>
#include <utility>

namespace mpdef
{
  template<typename ...T>
  struct list { };

  struct list_tag;

  struct make_list_fn
  {
    template<typename ...T>
    constexpr auto operator()(T...) const
    {
      return mpdef::list<T...>{};
    }
  };
  constexpr make_list_fn make_list{};

  // Comparable Operators

  template<typename... T, typename... U>
  constexpr auto operator==(mpdef::list<T...> t, mpdef::list<U...> u)
  {
    return boost::hana::equal(t, u);
  }

  template<typename... T, typename... U>
  constexpr auto operator!=(mpdef::list<T...> t, mpdef::list<U...> u)
  {
    return boost::hana::not_equal(t, u);
  }

  template <typename T>
  struct to_tuple_impl;

  template <typename ...Xs>
  struct to_tuple_impl<mpdef::list<Xs...>>
  {
    using type = boost::hana::tuple<Xs...>;
  };

  template <typename T>
  using to_tuple = typename to_tuple_impl<T>::type;

}//mpdef

namespace boost::hana
{

  template<typename ...T>
  struct tag_of<mpdef::list<T...>> { using type = mpdef::list_tag; };

  template<>
  struct make_impl<mpdef::list_tag>
  {
    template<typename... T>
    static constexpr auto apply(T...) 
      -> mpdef::list<T...>
    {
      return {};
    }
  };

  // Foldable

  template<>
  struct unpack_impl<mpdef::list_tag>
  {
    template<typename... T, typename F>
    static constexpr auto apply(mpdef::list<T...>, F&& f)
    {
      return std::forward<F>(f)(T{}...);
    }
  };

  // Functor

  template<>
  struct transform_impl<mpdef::list_tag>
  {
    template<typename... T, typename F>
    static constexpr auto apply(mpdef::list<T...>, F&& f)
      -> mpdef::list<decltype(f(T{}))...>
    {
      return {};
    }
  };

  // Iterable

  template<>
  struct at_impl<mpdef::list_tag>
  {
    template<typename... T, typename I>
    static constexpr auto apply(mpdef::list<T...>, I)
      -> decltype(hana::arg<I::value + 1>(T{}...))
    {
      return {};
    }
  };

  template<>
  struct is_empty_impl<mpdef::list_tag>
  {
    template<typename... T>
    static constexpr auto apply(mpdef::list<T...>)
      -> hana::bool_<sizeof...(T) == 0>
    {
      return {};
    }
  };

  template<>
  struct drop_front_impl<mpdef::list_tag>
  {
    template <std::size_t offset, typename Xs, std::size_t ...i>
    static constexpr auto helper(Xs, std::index_sequence<i...>)
      -> mpdef::list<decltype(hana::at_c<i + offset>(Xs{}))...>
    { return {}; }

    template<typename Xs, typename Size>
    static constexpr auto apply(Xs, Size)
    {
      return helper<Size::value>(
        Xs{},
        std::make_index_sequence<decltype(hana::length(Xs{}))::value - Size::value>{}
      );
    }
  };

  // MonadPlus

  template <>
  struct concat_impl<mpdef::list_tag>
  {
    template <typename ...Xs, typename ...Ys>
    static constexpr auto apply(mpdef::list<Xs...>, mpdef::list<Ys...>)
      -> mpdef::list<Xs..., Ys...>
    { return {}; }
  };

  // Sequence

  template<>
  struct Sequence<mpdef::list_tag>
  {
    static constexpr bool value = true;
  };
}

#endif
