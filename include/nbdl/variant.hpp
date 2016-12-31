//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_VARIANT_HPP
#define NBDL_VARIANT_HPP

#include <nbdl/detail/common_type.hpp>
#include <nbdl/fwd/bind_variant.hpp>

#include <type_traits>
#include <boost/hana.hpp>
#include <boost/hana/experimental/types.hpp>

namespace nbdl
{

  namespace hana = boost::hana;

  struct variant_tag { };

  namespace detail {

    template <typename DefaultType, typename... Tn>
    class variant
    {
      static_assert(std::is_empty<DefaultType>::value, "DefaultType must be an empty tag struct");
      using Storage = typename std::aligned_union<sizeof(DefaultType), DefaultType, Tn...>::type;

      using Types = hana::experimental::types<DefaultType, Tn...>;

      static auto type_ids()
      {
        return hana::unpack(hana::range_c<int, 1, sizeof...(Tn)+1>, [](auto ...i) {
          return hana::make_map(
            hana::make_pair(hana::type_c<DefaultType>, hana::int_c<0>),
            hana::make_pair(hana::type_c<Tn>, i)...
          );
        });
      }

      using LastTypeId = hana::int_<sizeof...(Tn)>;

      int type_id;
      Storage value_;

      template <typename T>
      constexpr int type_id_from_type() const
      {
        return *hana::find(type_ids(), hana::type_c<std::decay_t<T>>);
      }

      template <typename Fn>
      void call_by_type(const int value_type_id, Fn&& fn)
      {
        hana::for_each(
          hana::range_c<int, 0, hana::length(Types{})>,
          [&](auto i) {
            if (value_type_id == hana::value(i))
              fn(hana::at(Types{}, i));
          });
      }

      void copy(const int src_type_id, const void* src, void* dest)
      {
        call_by_type(src_type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          new (dest) T(*reinterpret_cast<const T*>(src));
        });
      }
      void move(const int src_type_id, void* src, void* dest)
      {
        call_by_type(src_type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          new (dest) T(std::move(*reinterpret_cast<T*>(src)));
        });
      }
      void destroy(const int value_type_id, void* value)
      {
        call_by_type(value_type_id, [&](auto matched) {
          using T = typename decltype(matched)::type;
          reinterpret_cast<T*>(value)->~T();
        });
      }

      template <typename Index, typename Fn>
      auto match_by_type_helper(Index i, const int type_id_x, Fn&& fn) const
        ->  nbdl::detail::common_type_t<
              decltype(fn(hana::at(Types{}, i))),
              decltype(fn(hana::at(Types{}, hana::int_c<0>)))
            >
      {
        if (type_id_x == Index::value)
          return std::forward<Fn>(fn)(hana::at(Types{}, i));
        else
          return match_by_type_helper(i + hana::int_c<1>, type_id_x, std::forward<Fn>(fn));
      }
      template <typename Fn>
      auto match_by_type_helper(LastTypeId i, const int type_id_x, Fn&& fn) const
        ->  nbdl::detail::common_type_t<
              decltype(fn(hana::at(Types{}, i))),
              decltype(fn(hana::at(Types{}, hana::int_c<0>)))
            >
      {
        //if type_id_x is invalid, use default, empty type
        if (type_id_x == LastTypeId::value)
          return std::forward<Fn>(fn)(hana::at(Types{}, i));
        else
          return std::forward<Fn>(fn)(hana::at(Types{}, hana::int_c<0>));
      }

      template <typename Fn1>
      decltype(auto) match_overload(Fn1&& fn1) const
      {
        return std::forward<Fn1>(fn1);
      }

      template <typename Fn1, typename Fn2, typename... Fns>
      auto match_overload(Fn1&& fn1, Fn2&& fn2, Fns&&... fns) const
      {
        return hana::overload_linearly(
          std::forward<Fn1>(fn1),
          std::forward<Fn2>(fn2),
          std::forward<Fns>(fns)...
        );
      }

      //provides a better compiler error that outputs 
      //the type in question should it be invalid
      template <typename Type>
      auto convert_from_type(Type&& val)
        -> std::enable_if_t<decltype(hana::contains(Types{}, hana::typeid_(val)))::value>
      {
        //it is critical that types are restricted to types supported by the Variant
        //this check is now redundant
        static_assert(decltype(hana::contains(Types{}, hana::typeid_(val)))::value,
          "This variant does not support conversion to Type.");

        // Do not destroy because this should
        // only be called from a constructor.
        new (&value_) std::decay_t<Type>(std::forward<Type>(val));
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

      variant& operator= (variant old)
      {
        destroy(type_id, &value_);
        move(old.type_id, &old.value_, &value_);
        type_id = old.type_id;
        return *this;
      }

      template <
        typename Type,
        typename = std::enable_if_t<!std::is_same<std::decay_t<Type>, variant>::value>
      >
      variant(Type&& val)
        : type_id(type_id_from_type<Type>())
      {
        convert_from_type(std::forward<Type>(val));
      }

      //for serialization
      int get_type_id() const
      {
        return type_id;
      }

      bool is_valid_type_id(int type_id_x) const
      {
        // perhaps this should just assume that type_ids are sequential indices
        if (type_id_x == 0)
        {
          return true;
        }
        else
        {
          return match_by_type(type_id_x,
            [&](auto type) {
              return (type_id_x == type_id_from_type<typename decltype(type)::type>());
            });
        }
      }

      //calls overload function with type type
      //used for serialization
      template <typename... Fns>
      auto match_by_type(const int type_id_x, Fns&&... fns) const
      {
        auto overload_ = match_overload(std::forward<Fns>(fns)...);

        //if type_id_x is invalid it will call with the default, empty type
        return match_by_type_helper(
            hana::int_c<0>,
            type_id_x,
            overload_
          );
      }

      template <typename T>
      bool is()
      {
        return (type_id == type_id_from_type<T>());
      }

      template <typename... Fns>
      auto match(Fns&&... fns) const
      {
        auto overload_ = match_overload(std::forward<Fns>(fns)...);

        return match_by_type_helper(hana::int_c<0>, type_id, [&](auto type) {
            using T = typename decltype(type)::type;
            return overload_(*reinterpret_cast<T const*>(&value_));
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

}//nbdl

#endif
