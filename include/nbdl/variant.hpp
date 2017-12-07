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

      public:
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
        : type_id(*type_id_from_type<T>())
      {
        // Do not destroy because this should
        // only be called from a constructor.
        new (&value_) std::decay_t<T>(std::move(val));
      }

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

      template <typename T, typename Fn>
      void unsafe_match_as(Fn&& fn) const
      {
        // not even checking T here
        std::forward<Fn>(fn)(*reinterpret_cast<T const*>(&value_));
      }

      template <typename T, typename Fn>
      void unsafe_match_as(Fn&& fn)
      {
        // not even checking T here
        std::forward<Fn>(fn)(*reinterpret_cast<T*>(&value_));
      }

      template <typename T>
      bool is()
      {
        return (type_id == int{*type_id_from_type<T>()});
      }

      template <typename T>
      using has = decltype(hana::is_just(type_id_from_type<T>()));

      template <typename... Fns>
      void match(Fns&&... fns) const
      {
        call_by_type(type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          hana::overload_linearly(std::forward<Fns>(fns)...)(
            *reinterpret_cast<T const*>(&value_)
          );
        });
      }

      template <typename... Fns>
      void match(Fns&&... fns)
      {
        call_by_type(type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          hana::overload_linearly(std::forward<Fns>(fns)...)(
            *reinterpret_cast<T*>(&value_)
          );
        });
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

        if constexpr(std::decay_t<Variant>::template has<T>::value)
        {
          if (v.template is<T>())
          {
            std::forward<Variant>(v).template unsafe_match_as<T>(std::forward<Fn>(fn));
          }
        }
      }
    };

#if 0 // not sure if this would really be a benefit
    template <>
    struct variant_visit_impl<mapped_overload_tag>
    {
      template <typename Variant, typename Visitor, typename Fn>
      static constexpr void apply(Variant&& v, Visitor const& visitor, Fn&& fn)
      {
        using Keys = decltype(hana::keys(visitor.map));
        hana::unpack(Keys{}, [&](auto ...keys)
        {
          // ???
        });
      }
    };
#endif
  }

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
      detail::variant_visit_impl<hana::tag_of_t<Fn>>::apply(
        std::forward<Store>(s)
      , fn
      , [&](auto&& value)
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
        }
      );
    }

    template <typename Store, typename Key, typename Fn>
    static constexpr void apply(Store&& s, Key&& k, Fn&& fn)
    {
      detail::variant_visit_impl<hana::tag_of_t<Fn>>::apply(
        std::forward<Store>(s)
      , fn
      , [&](auto&& value)
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
