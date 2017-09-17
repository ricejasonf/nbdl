//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_BIND_TAG_OF_HPP
#define NBDL_DETAIL_BIND_TAG_OF_HPP

#include <nbdl/concept/BindableMap.hpp>
#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/BindableVariant.hpp>
#include <nbdl/concept/Buffer.hpp>
#include <nbdl/concept/Container.hpp>
#include <nbdl/concept/DynamicBuffer.hpp>
#include <nbdl/concept/String.hpp>

#include <boost/hana/core/when.hpp>
#include <type_traits>

namespace nbdl::bind_detail
{
  namespace hana = boost::hana;

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

  template <typename T, typename = void>
  struct bto : bto<T, hana::when<true>> { };

  template <typename T, typename U = void>
  using bind_tag_of = typename bto<T, U>::type;

  template <typename T>
  struct bto<T, hana::when<
    (std::is_fundamental<T>::value || std::is_empty<T>::value)
    && !hana::Constant<T>::value
    && !hana::is_a<hana::type_tag, T>
    && !hana::is_a<hana::string_tag, T>
    && !nbdl::BindableSequence<T>::value
    && !nbdl::BindableMap<T>::value
    && !nbdl::BindableVariant<T>::value
    >>
  {
    using type = T;
  };

  template <typename T>
  struct bto<T, hana::when<hana::is_a<hana::type_tag, T>>>
  {
    using type = tag::type<typename T::type>;
  };

  template <template <typename...> class T>
  constexpr auto bind_tag_unpack_helper = [](auto const& ...x)
    -> T<bind_tag_of<std::decay_t<decltype(x)>>...>
  { return {}; };

  template <typename T>
  struct bto<T, hana::when<BindableSequence<T>::value>>
  {
    using type = decltype(
      nbdl::detail::bind_sequence_no_filter_fn{}(std::declval<T>(), bind_tag_unpack_helper<tag::sequence>)
    );
  };

  template <typename T>
  struct bto<T, hana::when<BindableVariant<T>::value>>
  {
    using type = typename decltype(
      hana::unpack(nbdl::bind_variant_types<T>, bind_tag_unpack_helper<tag::variant>)
    )::type;
  };

  constexpr auto bind_tag_unpack_map_helper = [](auto const& ...x)
    -> tag::map<tag::map_node<
                  bind_tag_of<std::decay_t<decltype(hana::first(x))>>,
                  bind_tag_of<typename std::decay_t<decltype(hana::second(x))>::type>
               >...>
  { return {}; };

  template <typename T>
  struct bto<T, hana::when<BindableMap<T>::value>>
  {
    using type = decltype(nbdl::bind_map(std::declval<T>(), bind_tag_unpack_map_helper));
  };

  template <typename T>
  struct bto<T, hana::when<hana::Constant<T>::value>>
  {
    using type = tag::constant<hana::value<T>()>;
  };

  template <char ...c>
  struct bto<hana::string<c...>> // hana::string representation is actually not supported :(
  {
    using type = tag::ct_string<c...>;
  };

  template <typename T>
  struct bto<T, hana::when<Container<T>::value
                        && !String<T>::value
                        && !Buffer<T>::value
                        && !DynamicBuffer<T>::value
                        >>
  {
    using type = tag::container<bind_tag_of<typename T::value_type>>;
  };

  template <typename T>
  struct bto<T, hana::when<Buffer<T>::value>>
  {
    using type = tag::buffer;
  };

  template <typename T>
  struct bto<T, hana::when<String<T>::value>>
  {
    using type = tag::string;
  };

  template <typename T>
  struct bto<T, hana::when<DynamicBuffer<T>::value && !String<T>::value>>
  {
    using type = tag::dynamic_buffer;
  };
}

#endif
