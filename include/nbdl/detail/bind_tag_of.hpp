//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_BIND_TAG_OF_HPP
#define NBDL_DETAIL_BIND_TAG_OF_HPP

#include <nbdl/concept/BindableBuffer.hpp>
#include <nbdl/concept/BindableDynamicBuffer.hpp>
#include <nbdl/concept/BindableDynamicSequence.hpp>
#include <nbdl/concept/BindableMap.hpp>
#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/BindableString.hpp>
#include <nbdl/concept/BindableVariant.hpp>

#include <type_traits>

namespace nbdl::detail
{
  namespace tag
  {
    template <typename ...>
    struct sequence { };

    template <typename ...>
    struct map { };

    template <typename ...>
    struct variant { };

    template <auto>
    struct constant { };

    template <typename>
    struct type { };

    template <typename>
    struct dynamic_sequence  { };

    struct buffer { };
    struct string { };
    struct dynamic_buffer { };
  }

  // bind_tag_of

  template <typename T, typename = void>
  struct bto<T>;

  template <typename T, typename U>
  using bind_tag_of = bto<T, U>;

  template <typename T, bool condition>
  struct bto<T, hana::when<condition>> { };

  // is_fundamental (type_traits)

  // TODO make this BindableValue
  template <typename T>
  struct bto<T, hana::when<std::is_fundamental<T>::value || std::is_empty<T>::value>>
  {
    using type = T;
  };

  template <typename T>
  struct bto<hana::type<T>>
  {
    using type = tag::type<T>;
  };

  template <template <typename...> class T>
  constexpr auto bind_tag_unpack_helper = [](auto const& ...x)
    -> T<bind_tag_of<decltype(hana::typeid_(x))::type>...>
  { return {}; };

  template <typename T>
  struct bto<T, hana::when<BindableSequence<T>::value>>
  {
    using type = decltype(
      nbdl::bind_sequence(std::declval<T>(), bind_tag_unpack_helper<tag::sequence>)
    );
  };

  template <typename T>
  struct bto<T, hana::when<BindableMap<T>::value>>
  {
    using type = decltype(nbdl::bind_map(std::declval<T>(), bind_tag_unpack_helper<tag::map>));
  };

  template <typename T>
  struct bto<T, hana::when<BindableVariant<T>::value>>
  {
    using type = decltype(
      hana::unpack(nbdl::bind_variant_types<T>, bind_tag_unpack_helper<tag::variant>)
    )::type;
  };

  template <typename T>
  struct bto<T, hana::when<hana::Constant<T>::value>>
  {
    using type = tag::constant<hana::value<T>()>;
  };

  template <typename T>
  struct bto<T, hana::when<BindableDynamicSequence<T>::value>>
  {
    using type = tag::dynamic_sequence<nbdl::bind_tag_of<nbdl::bind_dynamic_sequence_value_type<T>>>;
  };

  template <typename T>
  struct bto<T, hana::when<BindableBuffer<T>::value>>
  {
    using type = tag::buffer;
  };

  template <typename T>
  struct bto<T, hana::when<BindableString<T>::value>>
  {
    using type = tag::string;
  };

  template <typename T>
  struct bto<T, hana::when<BindableDynamicBuffer<T>::value>>
  {
    using type = tag::dynamic_buffer;
  };
}

#endif
