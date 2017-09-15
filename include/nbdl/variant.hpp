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

#include <boost/hana.hpp>
#include <boost/hana/experimental/types.hpp>
#include <boost/mp11/algorithm.hpp>
#include <type_traits>

namespace nbdl
{

  namespace hana = boost::hana;
  namespace mp11 = boost::mp11;

  struct variant_tag { };

  namespace detail
  {

    template <typename DefaultType, typename... Tn>
    class variant
    {
      static_assert(std::is_empty<DefaultType>::value, "DefaultType must be an empty tag struct");
      using Storage = typename std::aligned_union<sizeof(DefaultType), DefaultType, Tn...>::type;

      using Types = hana::experimental::types<DefaultType, Tn...>;

      int type_id;
      Storage value_;

      template <typename T>
      constexpr int type_id_from_type() const
      {
        return *hana::index_if(Types{}, hana::equal.to(hana::type_c<T>));
      }

      template <typename Fn>
      void call_by_type(int value_type_id, Fn const& fn) const
      {
        mp11::mp_with_index<sizeof...(Tn) + 1>(value_type_id, [&](auto index)
        {
          fn(hana::at(Types{}, index));
        });
      }

      void copy(int src_type_id, const void* src, void* dest)
      {
        call_by_type(src_type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          new (dest) T(*reinterpret_cast<const T*>(src));
        });
      }

      void move(int src_type_id, void* src, void* dest)
      {
        call_by_type(src_type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          new (dest) T(std::move(*reinterpret_cast<T*>(src)));
        });
      }

      void destroy(int value_type_id, void* value)
      {
        call_by_type(value_type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          reinterpret_cast<T*>(value)->~T();
        });
      }

      template <typename Fn>
      void match_by_type_helper(int value_type_id, Fn&& fn) const
      {
        call_by_type(value_type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          std::forward<Fn>(fn)(*reinterpret_cast<T const*>(&value_));
        });
      }

      public:

      using hana_tag = nbdl::variant_tag;

      variant()
        : type_id(0)
      { }

      variant(const variant& old)
        : type_id(old.type_id)
      {
        copy(old.type_id, &old.value_, &value_);
      }

      variant(variant&& old)
        : type_id(old.type_id)
      {
        move(old.type_id, &old.value_, &value_);
      }

      ~variant()
      {
        destroy(type_id, &value_);
      }

      variant& operator=(variant old)
      {
        destroy(type_id, &value_);
        move(old.type_id, &old.value_, &value_);
        type_id = old.type_id;
        return *this;
      }

      template <typename T>
      variant(T val)
        : type_id(type_id_from_type<T>())
      {
        // Do not destroy because this should
        // only be called from a constructor.
        new (&value_) std::decay_t<T>(std::move(val));
      }

      //for serialization
      int get_type_id() const
      {
        return type_id;
      }

      //calls overload function with type type
      //used for serialization
      template <typename Fn>
      void match_by_type(int type_id_x, Fn&& fn) const
      {
        call_by_type(type_id_x, std::forward<Fn>(fn));
      }

      template <typename T>
      bool is()
      {
        return (type_id == type_id_from_type<T>());
      }

      template <typename... Fns>
      auto match(Fns&&... fns) const
      {
        call_by_type(type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          hana::overload_linearly(std::forward<Fns>(fns)...)(
            *reinterpret_cast<T const*>(&value_)
          );
        });
      }
    };

  }//detail

  //useful for match catch all
  auto noop = [](auto){};

  //tags for empty variant value (always has type_id of 0)
  struct unresolved {};
  struct nothing {};

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
      std::forward<Store>(s).match([&](auto&& value)
      {
        if constexpr(nbdl::Store<decltype(value)>::value)
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
      });
    }

    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      std::forward<Store>(s).match([&](auto&& value)
      {
        if constexpr(nbdl::Store<decltype(value)>::value)
        {
          nbdl::match(
            std::forward<decltype(value)>(value)
          , std::forward<Key>(k)
          , std::forward<Fn>(fn)
          );
        }
        else
        {
          std::forward<Fn>(fn)(std::forward<decltype(value)>(value));
        }
      });
    }
  };

  template<>
  struct apply_action_impl<variant_tag>
  {
    template <typename Store, typename Other>
    static constexpr auto apply(Store& s, Other&& o)
    {
      s = std::forward<Other>(o);
      return hana::true_c;
    }
  };
}

#endif
