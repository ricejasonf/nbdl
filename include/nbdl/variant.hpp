//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_VARIANT_HPP
#define NBDL_VARIANT_HPP

#include <mpdef/map.hpp>
#include <mpdef/pair.hpp>
#include <nbdl/concept/Store.hpp>
#include <nbdl/detail/common_type.hpp>
#include <nbdl/fwd/bind_variant.hpp>
#include <nbdl/match.hpp>
#include <nbdl/tags.hpp>

#include <boost/hana.hpp>
#include <boost/hana/experimental/types.hpp>
#include <boost/mp11/algorithm.hpp>
#include <type_traits>

#include <variant>

namespace nbdl
{

  namespace hana = boost::hana;
  namespace mp11 = boost::mp11;

  struct variant_tag { };
  inline constexpr struct variant_index_t { } variant_index;

  namespace detail
  {

    template <typename DefaultType, typename... Tn>
    class variant
    {
      static_assert(std::is_empty<DefaultType>::value, "DefaultType must be an empty tag struct");
      //using Storage = typename std::aligned_union<sizeof(DefaultType), DefaultType, Tn...>::type;
      using Storage = std::variant<DefaultType, Tn...>;


      using Types = hana::experimental::types<DefaultType, Tn...>;

      public:
      Storage value_;
      template <typename T>
      static constexpr auto type_id_from_type()
      {
        return hana::index_if(Types{}, hana::equal.to(hana::type_c<T>));
      }
      private:

      template <typename Fn>
      void call_by_type(int value_type_id, Fn const& fn) const
      {
        mp11::mp_with_index<sizeof...(Tn) + 1>(value_type_id, [&](auto index)
        {
          fn(hana::at(Types{}, index));
        });
      }

      public:

      using hana_tag = nbdl::variant_tag;

      variant() = default;

      template <typename T>
      variant(T val)
        : value_(std::move(val))
      { }

      int get_type_id() const
      {
        return value_.index();
      }

      // calls overload function with type type
      // used for serialization
      template <typename Fn>
      void match_by_type(int type_id_x, Fn&& fn) const
      {
        if (!(type_id_x >= 0 && type_id_x < static_cast<int>(sizeof...(Tn) + 1)))
        {
          return;
        }

        call_by_type(type_id_x, std::forward<Fn>(fn));
      }

      template <typename T>
      constexpr bool is() const
      {
        return std::holds_alternative<T>(value_);
      }

      template <typename T>
      using has = decltype(hana::is_just(type_id_from_type<T>()));

      template <typename... Fns>
      void match(Fns&&... fns) const
      {
        std::visit(hana::overload_linearly(std::forward<Fns>(fns)...),
                   value_);
      }

      template <typename... Fns>
      void match(Fns&&... fns)
      {
        std::visit(hana::overload_linearly(std::forward<Fns>(fns)...),
                   value_);
      }
    };

    template <typename VisitorTag>
    struct variant_visit_impl
    {
      template <typename Variant, typename Visitor, typename Fn>
      static constexpr void apply(Variant&& v, Visitor const&, Fn&& fn)
      {
        std::forward<Variant>(v).match(std::forward<Fn>(fn));
      }
    };

    template <>
    struct variant_visit_impl<match_when_tag>
    {
      template <typename Variant, typename Visitor, typename Fn>
      static constexpr void apply(Variant&& v, Visitor const&, Fn&& fn)
      {
        using T = typename Visitor::type;

        // Note match_when does nothing for invalid alternatives.
        if constexpr(std::decay_t<Variant>::template has<T>::value)
        {
          if (v.template is<T>())
          {
            std::forward<Fn>(fn)(std::get<T>(std::forward<Variant>(v).value_));
          }
        }
      }
    };
  }

  // tag for empty variant value (always has type index of 0)
  struct nothing { };

  template <typename... Tn>
  using variant = detail::variant<unresolved, Tn...>;

  template <typename T>
  using optional = detail::variant<nothing, T>;

  struct make_optional_fn
  {
    template <typename T>
    auto operator()(T&& value) const
    {
      return nbdl::optional<std::decay_t<T>>(value);
    }
  };

  constexpr make_optional_fn make_optional;

  struct make_optional_if_fn
  {
    template <typename T>
    auto operator()(bool cond, T&& value) const
    {
      using Optional = nbdl::optional<std::decay_t<T>>;

      if (cond)
      {
        return Optional(value);
      }
      else
      {
        return Optional(nbdl::nothing{});
      }
    }
  };

  constexpr make_optional_if_fn make_optional_if;

  // BindableVariant

  template <>
  struct bind_variant_impl<nbdl::variant_tag>
  {
    template <typename Variant, typename BindFn>
    static void apply(int type_id, Variant&& v, BindFn&& f)
    {
      v.match_by_type(type_id, [&](auto type)
      {
        using T = typename decltype(type)::type;
        T value{};
        f(type_id, value);
        v = value;
      });
    }

    template <typename Variant, typename BindFn>
    static void apply(Variant const& v, BindFn&& f)
    {
      v.match([&](auto const& value)
      {
        f(v.get_type_id(), value);
      });
    }
  };

  template <typename ...T>
  auto bind_variant_types<nbdl::variant<T...>> = hana::experimental::types<T...>{};

  // Store

  template<>
  struct match_impl<variant_tag>
  {
    template <typename Store, typename Fn>
    static constexpr void apply(Store&& s, Fn&& fn)
    {
      detail::variant_visit_impl<hana::tag_of_t<Fn>>::apply(
        std::forward<Store>(s)
      , fn
      , [&](auto&& value)
        {
          if constexpr(nbdl::Store<decltype(value)>)
          {
            nbdl::match(
              std::forward<decltype(value)>(value)
            , std::forward<Fn>(fn)
            );
          }
          else
          {
            std::forward<Fn>(fn)(std::forward<decltype(value)>(value));
          }
        }
      );
    }

    template <typename Store, typename Fn>
    static constexpr void apply(Store&& s, variant_index_t, Fn&& fn)
    {
      std::forward<Fn>(fn)(s.get_type_id());
    }

    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      detail::variant_visit_impl<hana::tag_of_t<Fn>>::apply(
        std::forward<Store>(s)
      , fn
      , [&](auto&& value)
        {
          if constexpr(nbdl::Store<decltype(value)>)
          {
            nbdl::match(
              std::forward<decltype(value)>(value)
            , std::forward<Key>(k)
            , std::forward<Fn>(fn)
            );
          }
          else
          {
            // FIXME This feels incorrect. What happened to the key?
            //       I think specifying a key should require get/match
            //       be applied with the key to the value
            //       (which is handled by nbdl::match above.)
            std::forward<Fn>(fn)(std::forward<decltype(value)>(value));
          }
        }
      );
    }
  };
}

namespace boost::hana
{
  // Comparable

  template <>
  struct equal_impl<nbdl::variant_tag, nbdl::variant_tag>
  {
    template <typename T>
    static bool apply(T const& t1, T const& t2)
    {
      if (t1.get_type_id() == t2.get_type_id())
      {
        bool result = false;
        t1.match(hana::overload_linearly(
          [&](auto const& v1)
          {
            using V = std::decay_t<decltype(v1)>;
            t2.match(hana::overload_linearly(
              [&](V const& v2)
              {
                if constexpr(std::is_empty<V>::value)
                {
                  result = true;
                }
                else
                {
                  result = hana::equal(v1, v2);
                }
              }
            , nbdl::noop
            ));
          }
        , nbdl::noop
        ));

        return result;
      }

      return false;
    }
  };
}

#endif
