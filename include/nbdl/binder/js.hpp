//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDER_JS_HPP
#define NBDL_BINDER_JS_HPP

#include <nbdl/bind_map.hpp>
#include <nbdl/bind_sequence.hpp>
#include <nbdl/bind_variant.hpp>
#include <nbdl/concept/BindableMap.hpp>
#include <nbdl/concept/BindableSequence.hpp>
#include <nbdl/concept/BindableVariant.hpp>
#include <nbdl/concept/String.hpp>
#include <nbdl/detail/js_val.hpp>
#include <nbdl/js.hpp>
#include <nbdl/string.hpp>
#include <nbdl/util/base64_encode.hpp>
#include <nbdl/util/base64_decode.hpp>
#include <nbdl/variant.hpp>

#include <boost/hana/ext/std/tuple.hpp>
#include <emscripten.h>
#include <string>
#include <type_traits>
#include <utility>

namespace nbdl::binder::js
{
  namespace hana = boost::hana;
  using js_val = nbdl::detail::js_val;

  struct bind_to_fn
  {
    template <typename X>
    void operator()(nbdl::js::val&, X const&) const;

    template <typename X>
    void operator()(js_val&, X const&) const;
  };

  constexpr bind_to_fn bind_to{};

  struct bind_from_fn
  {
    template <typename X>
    void operator()(nbdl::js::val const&, X&) const;

    template <typename X>
    void operator()(js_val const&, X&) const;
  };

  constexpr bind_from_fn bind_from{};

  namespace detail
  {

    template <typename T>
    struct bind_to_impl;

    template <typename T>
    struct bind_from_impl;
  }

  template <typename X>
  void bind_to_fn::operator()(nbdl::js::val& v, X const& x) const
  {
    (*this)(reinterpret_cast<js_val&>(v), x);
  }

  template <typename X>
  void bind_to_fn::operator()(js_val& val, X const& x) const
  {
    using Tag = hana::tag_of_t<X>;
    using Impl = detail::bind_to_impl<Tag>;

    Impl::apply(val, x);
  }

  template <typename X>
  void bind_from_fn::operator()(nbdl::js::val const& v, X& x) const
  {
    (*this)(reinterpret_cast<js_val const&>(v), x);
  }

  template <typename X>
  void bind_from_fn::operator()(js_val const& val, X& x) const
  {
    using Tag = hana::tag_of_t<X>;
    using Impl = detail::bind_from_impl<Tag>;

    Impl::apply(val, x);
  }

  namespace detail
  {
    int get_variant_type_id(js_val const& val)
    {
      return EM_ASM_INT(
        {
          var v = Module.NBDL_DETAIL_JS_GET($0);
          if (Array.isArray(v))
          {
            return v[0];
          }
          else
          {
            return v;
          }
        }
      , val.handle()
      );
    }

    //
    // bind_to_impl
    //

    template <nbdl::String T>
    struct bind_to_impl<T> {
      template <typename X>
      static void apply(js_val& val, X const& x) {
        EM_ASM_(
          {
            Module.NBDL_DETAIL_JS_SET($0, UTF8ToString($1, $2));
          }
        , val.handle()
        , x.data()
        , x.size()
        );
      }
    };

    template <typename T>
      requires std::is_integral<T>::value
    struct bind_to_impl<T> {
      template <typename X>
      static void apply(js_val& val, X x) {
        EM_ASM_(
          {
            Module.NBDL_DETAIL_JS_SET($0, $1);
          }
        , val.handle()
        , x
        );
      }
    };

    template <typename T>
      requires std::is_enum<T>::value
    struct bind_to_impl<T> {
      template <typename X>
      static void apply(js_val& val, X x) {
        EM_ASM_(
          {
            Module.NBDL_DETAIL_JS_SET($0, $1);
          }
        , val.handle()
        , static_cast<std::underlying_type_t<X>>(x)
        );
      }
    };

    template <nbdl::BindableSequence T>
      requires (!nbdl::BindableMap<T>)
    struct bind_to_impl<T> {
      template <typename Sequence>
      static void apply(js_val& val, Sequence const& sequence) {
        js_val child_val{};
        EM_ASM_({ Module.NBDL_DETAIL_JS_SET($0, []); }, val.handle());

        nbdl::bind_sequence(sequence, hana::overload(
          []()
          {
            // no members... do nothing
          }
        , [&](auto& x)
          {
            // singleton does not need to be wrapped in array
            bind_to(val, x);
          }
        , [&](auto& x1, auto& x2, auto& ...xs)
          {
            hana::for_each(std::forward_as_tuple(x1, x2, xs...),  [&](auto& x)
            {
              bind_to(child_val, x);
              EM_ASM_(
                {
                  Module.NBDL_DETAIL_JS_GET($0).push(
                    Module.NBDL_DETAIL_JS_GET($1)
                  );
                }
              , val.handle()
              , child_val.handle()
              );
            });
          }
        ));
      }
    };

    template <nbdl::Container T>
    struct bind_to_impl<T> {
      template <typename Xs>
      static void apply(js_val& val, Xs const& xs)
      {
        js_val child_val{};
        EM_ASM_({ Module.NBDL_DETAIL_JS_SET($0, []); }, val.handle());
        for (auto const& x: xs)
        {
          bind_to(child_val, x);
          EM_ASM_(
            {
              Module.NBDL_DETAIL_JS_GET($0).push(
                Module.NBDL_DETAIL_JS_GET($1)
              );
            }
          , val.handle()
          , child_val.handle()
          );
        };
      }
    };

    template <ContiguousByteContainer T>
    struct bind_to_impl<T> {
      template <typename Xs>
      static void apply(js_val& val, Xs const& xs)
      {
        nbdl::string temp = nbdl::string(nbdl::util::base64_encode(xs));
        bind_to(val, temp);
      }
    };

    template <BindableMap T>
    struct bind_to_impl<T> {
      template <typename Xs>
      static void apply(js_val& val, Xs const& xs)
      {
        js_val child_val{};
        EM_ASM_({ Module.NBDL_DETAIL_JS_SET($0, {}); }, val.handle());

        nbdl::bind_map(xs, [&](auto const& ...y)
        {
          hana::for_each(std::forward_as_tuple(y...), [&](auto& pair)
          {
            bind_to(child_val, hana::second(pair).get());
            EM_ASM_(
              {
                var key = UTF8ToString($1, $2);
                Module.NBDL_DETAIL_JS_GET($0)[key] = Module.NBDL_DETAIL_JS_GET($3);
              }
            , val.handle()
            , hana::first(pair).c_str()
            , decltype(hana::size(hana::first(pair)))::value
            , child_val.handle()
            );
          });
        });
      }
    };

    template <nbdl::BindableVariant T>
    struct bind_to_impl<T> {
      template <typename Variant>
      static void apply(js_val& val, Variant const& v)
      {
        nbdl::bind_variant(v, [&](auto const& ...x)
        {
          auto refs = std::forward_as_tuple(x...);
          bind_to(val, refs);
        });
      }
    };

    //
    // bind_from_impl
    //

    template <nbdl::String T>
    struct bind_from_impl<T> {
      template <typename X>
      static void apply(js_val const& val, X& x)
      {
        using String = std::decay_t<X>;

        int length = EM_ASM_INT(
          {
            return Module.NBDL_DETAIL_JS_GET($0).length;
          }
        , val.handle()
        );

        String temp(length, '\0');

        EM_ASM_(
          {
            var v = Module.NBDL_DETAIL_JS_GET($0);
            if (typeof v === 'string')
            {
              Module.stringToAscii(v, $1);
            }
            else // assume its an ArrayBuffer
            {
              var heap = new Uint8Array(Module.HEAPU8.buffer, $1, v.length);
              heap.set(v);
            }
          }
        , val.handle()
        , temp.data()
        );

        x = std::move(temp);
      }
    };

    template <typename T>
      requires std::is_integral<T>::value
    struct bind_from_impl<T> {
      template <typename X>
      static void apply(js_val const& val, X& x)
      {
        x = EM_ASM_INT(
          {
            return Module.NBDL_DETAIL_JS_GET($0);
          }
        , val.handle()
        );
      }
    };

    template <typename T>
      requires std::is_enum<T>::value
    struct bind_from_impl<T> {
      template <typename X>
      static void apply(js_val const& val, X& x)
      {
        x = static_cast<X>(EM_ASM_INT(
          {
            return Module.NBDL_DETAIL_JS_GET($0);
          }
        , val.handle()
        ));
      }
    };

    template <nbdl::BindableSequence T>
      requires (!nbdl::BindableMap<T>)
    struct bind_from_impl<T> {
      template <typename Sequence>
      static void apply(js_val const& val, Sequence& sequence)
      {
        js_val child_val{};
        int i = 0;

        nbdl::bind_sequence(sequence, hana::overload(
          []()
          {
            // no members... do nothing
          }
        , [&](auto& x)
          {
            // singleton does not need to be wrapped in array
            bind_from(val, x);
          }
        , [&](auto& x1, auto& x2, auto& ...xs)
          {
            hana::for_each(std::forward_as_tuple(x1, x2, xs...),  [&](auto& x)
            {
              EM_ASM_(
                {
                  Module.NBDL_DETAIL_JS_SET($1,
                    Module.NBDL_DETAIL_JS_GET($0)[$2]
                  );
                }
              , val.handle()
              , child_val.handle()
              , i
              );

              bind_from(child_val, x);

              ++i;
            });
          }
        ));
      }
    };

    template <nbdl::Container T>
    struct bind_from_impl<T> {
      template <typename Xs>
      static void apply(js_val const& val, Xs& xs)
      {
        using Container = std::decay_t<Xs>;

        int length = EM_ASM_INT(
          {
            return Module.NBDL_DETAIL_JS_GET($0).length;
          }
        , val.handle()
        );

        Container container(length);
        int i = 0;
        js_val child_val{};

        for (auto& x : container)
        {
          EM_ASM_(
            {
              Module.NBDL_DETAIL_JS_SET($1,
                Module.NBDL_DETAIL_JS_GET($0)[$2]
              );
            }
          , val.handle()
          , child_val.handle()
          , i
          );

          bind_from(child_val, x);

          ++i;
        };

        xs = std::move(container);
      }
    };

    template <nbdl::ContiguousByteContainer T>
    struct bind_from_impl<T> {
      template <typename Xs>
      static void apply(js_val const& val, Xs& xs) {
        nbdl::string temp{};
        bind_from(val, temp);
        // TODO actually check for error
        nbdl::util::base64_decode(temp, xs);
      }
    };

    template <nbdl::BindableMap T>
    struct bind_from_impl<T> {
      template <typename Xs>
      static void apply(js_val const& val, Xs& xs) {
        js_val child_val{};

        nbdl::bind_map(xs, [&](auto ...y)
        {
          hana::for_each(std::forward_as_tuple(y...), [&](auto pair)
          {
            EM_ASM_(
              {
                Module.NBDL_DETAIL_JS_SET($1,
                  Module.NBDL_DETAIL_JS_GET($0)[UTF8ToString($2, $3)]
                );
              }
            , val.handle()
            , child_val.handle()
            , hana::first(pair).c_str()
            , decltype(hana::size(hana::first(pair)))::value
            );

            bind_from(child_val, hana::second(pair).get());
          });
        });
      }
    };

    template <nbdl::BindableVariant T>
    struct bind_from_impl<T>
    {
      template <typename Variant>
      static void apply(js_val const& val, Variant& var)
      {
        js_val child_val{};

        int type_id = get_variant_type_id(val);
        nbdl::bind_variant(type_id, var, [&](auto& ...x)
        {
          auto refs = std::forward_as_tuple(x...);
          bind_from(val, refs);
        });
      }
    };
  }
}

#endif
