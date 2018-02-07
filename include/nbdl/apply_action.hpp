//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_APPLY_ACTION_HPP
#define NBDL_APPLY_ACTION_HPP

#include <nbdl/fwd/apply_action.hpp>

#include <nbdl/concept/NetworkStore.hpp>
#include <nbdl/concept/Store.hpp>
#include <nbdl/fwd/apply_message.hpp>

#include <boost/hana/concept/searchable.hpp>
#include <boost/hana/core/default.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/integral_constant.hpp>
#include <functional>
#include <type_traits>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

  template <typename Store, typename Message>
  constexpr auto apply_action_fn::operator()(Store&& s, Message&& m) const
  {
    using Tag = hana::tag_of_t<Store>;
    using Impl = apply_action_impl<Tag>;

#if 0 // TODO This would make a useful early error message
    static_assert(
      !hana::is_convertible<
        decltype(Impl::apply(std::forward<Store>(s), std::forward<Message>(m)))
      , bool
      >::value
    , "nbdl::apply_action must return a value convertible to bool"
    );
#endif
    return Impl::apply(std::forward<Store>(s), std::forward<Message>(m));
  };

  template <typename Tag, bool condition>
  struct apply_action_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    template <typename Store, typename Action>
    static constexpr auto apply(Store& s, Action&& a)
    {
      if constexpr(std::is_assignable<Store&, Action>::value)
      {
        s = std::forward<Action>(a);
        return hana::true_c;
      }
      else
      {
        std::forward<Action>(a)(s);
        return hana::true_c;
      }
    }
  };

  template <typename Tag>
  struct apply_action_impl<Tag, hana::when<nbdl::NetworkStore<Tag>::value>>
  {
    template <typename Store, typename Action>
    static constexpr auto apply(Store& s, Action&& a)
    {
      return nbdl::apply_message(s , std::forward<Action>(a));
    }
  };

  template <typename T>
  struct apply_action_impl<std::reference_wrapper<T>>
  {
    template <typename Store, typename Action>
    static constexpr auto apply(Store s, Action&& a)
    {
      return nbdl::apply_action(s.get(), std::forward<Action>(a));
    }
  };
}

#endif
