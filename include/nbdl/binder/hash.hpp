//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDER_HASH_HPP
#define NBDL_BINDER_HASH_HPP

#include <nbdl/bind_map.hpp>
#include <nbdl/bind_sequence.hpp>
#include <nbdl/concept/BindableMap.hpp>
#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/BindableVariant.hpp>
#include <nbdl/detail/hash_combine.hpp>

#include <functional>

namespace nbdl::binder
{
  namespace detail {

    struct writer
    {
      std::size_t digest;

      writer() :
        digest(0)
      { }

      template <typename T>
      void bind(T const& t)
      {
        if constexpr(nbdl::BindableSequence<T>::value)
        {
          nbdl::bind_sequence(t, [&](auto const& ...xs)
          {
            hana::for_each(hana::make_tuple(std::cref(xs)...), [&](auto const& x)
            {
              bind(x.get());
            });
          });
        }
        else if constexpr(nbdl::BindableVariant<T>::value)
        {
          nbdl::bind_variant(t, [&](auto const& type_id, auto const& value)
          {
            bind(type_id);
            bind(value); 
          });
        }
        else
        {
          nbdl::detail::hash_combine(digest, t);
        }
      }
    };
  }

  struct hash_fn
  {
    template <typename T>
    std::size_t operator()(T const& t) const
    {
      detail::writer binder{};
      binder.bind(t);
      return binder.digest;
    }
  };

  constexpr hash_fn hash{};
}

#endif
