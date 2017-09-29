//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BIND_SEQUENCE_HPP
#define NBDL_BIND_SEQUENCE_HPP

#include <nbdl/fwd/bind_sequence.hpp>

#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/Entity.hpp>
#include <nbdl/detail/filter_empties.hpp>
#include <nbdl/entity_members.hpp>

#include <boost/hana/concept/product.hpp>
#include <boost/hana/concept/sequence.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/fwd/optional.hpp>
#include <boost/hana/ext/std/pair.hpp> // to support std map containers
#include <type_traits>
#include <utility>

namespace nbdl
{
  template<typename BindableSequence, typename BindFn>
  constexpr auto bind_sequence_fn::operator()(BindableSequence&& s, BindFn&& f) const
  {
    using Tag = hana::tag_of_t<BindableSequence>;
    using Impl = bind_sequence_impl<Tag>;

    static_assert(nbdl::BindableSequence<BindableSequence>::value,
      "nbdl::bind_sequence(seq, fn) requires 'seq' to be a BindableSequence");

    return Impl::apply(
      std::forward<BindableSequence>(s),
      detail::filter_empties_fn<std::decay_t<BindFn>>{std::forward<BindFn>(f)}
    );
  };

  namespace detail
  {
    // bleh
    struct bind_sequence_no_filter_fn
    {
      template<typename BindableSequence, typename BindFn>
      constexpr auto operator()(BindableSequence&& s, BindFn&& f) const
      {
        using Tag = hana::tag_of_t<BindableSequence>;
        using Impl = bind_sequence_impl<Tag>;

        static_assert(nbdl::BindableSequence<BindableSequence>::value,
          "nbdl::bind_sequence(seq, fn) requires 'seq' to be a BindableSequence");

        return Impl::apply(
          std::forward<BindableSequence>(s),
          std::forward<BindFn>(f)
        );
      }
    };
  }

  template<typename Tag, bool condition>
  struct bind_sequence_impl<Tag, hana::when<condition>>
    : hana::default_
  {
    static constexpr auto apply(...) = delete;
  };

  template<typename Tag>
  struct bind_sequence_impl<Tag, hana::when<nbdl::Entity<Tag>::value>>
  {
    template <typename Entity, typename BindFn>
    static constexpr auto apply(Entity&& e, BindFn&& f)
    {
      return hana::unpack(nbdl::entity_members<Entity>, [&](auto ...m)
      {
        return f(nbdl::get_member<decltype(m)>(e)...);
      });
    }
  };

  // Support for hana::Foldables that guarantee
  // deterministic ordering.
  template<typename Tag>
  struct bind_sequence_impl<Tag, hana::when<
        hana::Sequence  <Tag>::value
    ||  hana::Product   <Tag>::value
    ||  hana::is_a<hana::optional_tag, Tag>
  >>
  {
    template <typename BindableSequence, typename BindFn>
    static constexpr auto apply(BindableSequence&& xs, BindFn&& f)
    {
      return hana::unpack(std::forward<BindableSequence>(xs), [&](auto&& ...x)
      {
        return std::forward<BindFn>(f)(std::forward<decltype(x)>(x)...);
      });
    }
  };
}

#endif
