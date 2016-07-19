//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DELTA_HPP
#define NBDL_DELTA_HPP

#include<nbdl/fwd/delta.hpp>

#include<nbdl/make_delta.hpp>
#include<nbdl/apply_delta.hpp>
#include<nbdl/variant.hpp>

namespace nbdl
{
  namespace hana = boost::hana;

  namespace detail
  {
    struct apply_diff_value_fn
    {
      template <typename X, typename Option>
      auto operator()(X&& x, Option const& opt)
      {
        opt.match(
          [](nbdl::nothing) { },
          [&](auto const& value)
          {
            std::forward<X>(x) = value;
          }
        );
      }
    };
  }

  template <typename Bindable>
  struct delta
  {
    using hana_tag = delta_tag;
    using storage_t = decltype(nbdl::bind_sequence(std::declval<Bindable>(),
      hana::make_basic_tuple ^hana::on^ nbdl::make_optional));

    storage_t storage;

    delta(storage_t const& s)
      : storage(s)
    { }

    delta(storage_t& s)
      : storage(s)
    { }

    delta(storage_t&& s)
      : storage(std::move(s))
    { }
  };

  template <>
  struct make_delta_impl<nbdl::delta_tag>
  {
    template <typename E1, typename E2>
    static auto apply(E1 const& e1, E2 const& e2)
    {
      using Bindable = std::decay_t<E1>;
      using Delta = nbdl::delta<Bindable>;

      return nbdl::bind_sequence(e1, [&](auto const& ...e1_arg)
      {
        return nbdl::bind_sequence(e2, [&e1_arg ...](auto const& ...e2_arg)
        {
          return Delta(hana::make_basic_tuple(
            nbdl::make_optional_if(hana::not_(hana::equal(e1_arg, e2_arg)), e1_arg)...
          ));
        });
      });
    }
  };

  template <>
  struct apply_delta_impl<nbdl::delta_tag>
  {
    template <typename Delta, typename Bindable>
    static auto apply(Delta&& d, Bindable&& b)
    {
      static_assert(
        decltype(
          hana::typeid_(d) == hana::type_c<nbdl::delta<std::decay_t<Bindable>>>
        )::value
        , "nbdl::delta must be applied to the same Bindable type.");
      nbdl::bind_sequence(b, [&](auto&& ...xs)
      {
        hana::unpack(d.storage, [&xs...](auto&& ...opts)
        {
          (detail::apply_diff_value_fn{}(
            std::forward<decltype(xs)>(xs),
            std::forward<decltype(opts)>(opts)
          ), ...);
        });
      });
      return std::forward<Bindable>(b);
    }
  };
} // nbdl

#endif
