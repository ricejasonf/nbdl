//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_BIND_TAG_OF_HPP
#define NBDL_DETAIL_BIND_TAG_OF_HPP

#include <nbdl/bind_map.hpp>
#include <nbdl/bind_sequence.hpp>
#include <nbdl/bind_variant.hpp>
#include <nbdl/concept/BindableMap.hpp>
#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/BindableVariant.hpp>
#include <nbdl/concept/Buffer.hpp>
#include <nbdl/concept/Container.hpp>
#include <nbdl/concept/DynamicBuffer.hpp>
#include <nbdl/concept/String.hpp>

#include <boost/hana/core/when.hpp>
#include <boost/hana/unpack.hpp>
#include <type_traits>

namespace nbdl::_b
{
  namespace hana = boost::hana;

  // Changing the names of these tags is a breaking change
  // This includes namespaces and anything that shows up
  // in the symbols.
  namespace tag
  {
    template <typename ...>
    struct sequence { };

    template <typename ...>
    struct map { };

    template <typename ...>
    struct map_node { };

    template <typename ...>
    struct variant { };

    template <auto>
    struct constant { };

    template <typename>
    struct type { };

    template <char...>
    struct ct_string { };

    template <typename>
    struct container  { };

    struct buffer { };
    struct string { };
    struct dynamic_buffer { };
  }

  // bind_tag_of

  template <typename T>
  struct bto {
    // all others should be specialized
    static_assert(std::is_fundamental<T>::value || std::is_empty<T>::value);
    using type = T;
  };

  template <typename T>
  using bind_tag_of = typename bto<T>::type;

  template <typename T>
    requires (hana::is_a<hana::type_tag, T>())
  struct bto<T> {
    using type = tag::type<typename T::type>;
  };

  struct bind_tag_unpack_helper {
    template <typename ...X>
    constexpr auto operator()(X&&...) const
      -> tag::sequence<bind_tag_of<std::decay_t<X>>...>
    { return {}; }
  };

  template <BindableSequence T>
  struct bto<T> {
    using type = std::invoke_result_t<nbdl::detail::bind_sequence_no_filter_fn, T,
                                     bind_tag_unpack_helper>;
  };

  template <typename ...T>
  struct bind_tag_unpack_variant_types_helper {
    using type = tag::variant<bind_tag_of<T>...>;
  };

  template <BindableVariant T>
  struct bto<T> {
    using type = typename decltype(hana::unpack(
      nbdl::bind_variant_types<T>, hana::metafunction<bind_tag_unpack_variant_types_helper>
    ))::type;
  };

  struct bind_tag_unpack_map_helper {
    template <typename ...X>
    constexpr auto operator()(X&&... x) const
      -> tag::map<tag::map_node<
            bind_tag_of<std::decay_t<decltype(hana::first(x))>>,
            bind_tag_of<typename std::decay_t<decltype(hana::second(x))>::type>
          >...>
    { return {}; }
  };

  template <BindableMap T>
  struct bto<T> {
    using type = std::invoke_result_t<nbdl::bind_map_fn, T, bind_tag_unpack_map_helper>;
  };

  template <typename T>
    requires hana::Constant<T>::value
  struct bto<T> {
    using type = tag::constant<hana::value<T>()>;
  };

  template <char ...c>
  struct bto<hana::string<c...>> {
    using type = tag::ct_string<c...>;
  };

  template <Container T>
  struct bto<T> {
    using type = tag::container<bind_tag_of<typename T::value_type>>;
  };

  template <Buffer T>
  struct bto<T> {
    using type = tag::buffer;
  };

  template <String T>
  struct bto<T> {
    using type = tag::string;
  };

  template <DynamicBuffer T>
  struct bto<T> {
    using type = tag::dynamic_buffer;
  };
}

namespace nbdl::detail
{
  template <typename T>
  using bind_tag_of = _b::bind_tag_of<T>;
}

#endif
