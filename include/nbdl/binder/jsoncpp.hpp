//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_BINDER_JSONCPP_HPP
#define NBDL_BINDER_JSONCPP_HPP

#include<nbdl/concept/BindableMap.hpp>
#include<nbdl/concept/BindableSequence.hpp>
#include<nbdl/concept/BindableVariant.hpp>
#include<nbdl/concept/Container.hpp>
#include<nbdl/concept/String.hpp>
#include<nbdl/bind_map.hpp>
#include<nbdl/bind_sequence.hpp>
#include<nbdl/string.hpp>
#include<nbdl/util/base64_decode.hpp>
#include<nbdl/util/base64_encode.hpp>

#include<json/json.h>
#include<string>
#include<utility>

namespace nbdl { namespace binder { namespace jsoncpp
{
  namespace detail {
    template <typename BindFn>
    class reader
    {
      // JsonCpp leaves a little to be desired for
      // generic conversion. Should have `as<T>()`
      void bind_(Json::Value const& obj, bool& field)
      {
        if (!obj.isBool())
          throw std::runtime_error("JSON Boolean expected");
        field = obj.asBool();
      }

      void bind_(Json::Value const& obj, char& field)
      {
        if (!obj.isIntegral())
          throw std::runtime_error("JSON Integral expected");
        field = obj.asUInt();
      }

      void bind_(Json::Value const& obj, unsigned int& field)
      {
        if (!obj.isIntegral())
          throw std::runtime_error("JSON Integral expected");
        field = obj.asUInt();
      }

      void bind_(Json::Value const& obj, int& field)
      {
        if (!obj.isIntegral())
          throw std::runtime_error("JSON Integral expected");
        field = obj.asInt();
      }

      void bind_(Json::Value const& obj, double& field)
      {
        if (!obj.isNumeric())
          throw std::runtime_error("JSON Number expected");
        field = obj.asDouble();
      }

      Json::Value const& json_val;

      public:

      reader(Json::Value const& value) :
        json_val(value)
      { }

      template<typename T>
      void bind_member(T&& field)
      {
        bind_(json_val, std::forward<T>(field));
      }

      template <typename Container>
      void bind_container(Container& c)
      {
        if (!json_val.isArray())
          throw std::runtime_error("JSON Array expected");

        typename Container::value_type temp;

        for (Json::Value const& val : json_val)
        {
          temp = {};
          BindFn{}(reader(val), temp);
          c.insert(c.end(), std::move(temp));
        }
      }

      template <typename Buffer>
      void bind_buffer(Buffer& b)
      {
        nbdl::string temp{};
        bind_string(temp);
        b = nbdl::util::base64_decode(temp);
      }

      template <typename String>
      void bind_string(String& field)
      {
        if (!json_val.isString())
          throw std::runtime_error("JSON String expected");
        field = String{json_val.asCString()};
      }

      template<typename T>
      void bind_named_member(char const* name, T&& field)
      {
        BindFn{}(reader(json_val[name]), std::forward<T>(field));
      }

      void bind_sequence()
      {
        // optimized for nullary sequences
        // do nothing
      }

      template<typename X>
      void bind_sequence(X&& x)
      {
        // optimized for unary sequences
        BindFn{}(*this, std::forward<X>(x));
      }
      
      template<typename X1, typename X2, typename... Xs>
      void bind_sequence(X1&& x1, X2&& x2, Xs&&... xs)
      {
        if (!json_val.isArray() || json_val.size() != sizeof...(Xs) + 2)
          throw std::runtime_error("JSON Array expected when binding sequence.");
        auto iter = json_val.begin();
        BindFn{}(reader(*iter++), std::forward<X1>(x1));
        BindFn{}(reader(*iter++), std::forward<X2>(x2));
        hana::for_each(
          hana::make_tuple(std::ref(xs)...),
          [&](auto&& x) {
            BindFn{}(reader(*iter++), x.get());
          });
        // c++1z
        // (BindFn{}(reader(*iter++), xs), ...);
      }

      int get_variant_type_id()
      {
        if (json_val.isIntegral())
        {
          return json_val.asInt();
        }
        else if (json_val.isArray() && json_val.size() == 2)
        {
          if (!json_val[0u].isIntegral())
            throw std::runtime_error("Variant type_id is not integer.");
          return json_val[0u].asInt();
        }
        throw std::runtime_error("Variant type_id is invalid.");
      }
    };

    template <typename BindFn>
    class writer
    {
      Json::Value& json_val;

      public:

      writer(Json::Value& value) :
        json_val(value)
      {}

      template<class X>
      void bind_member(X&& x)
      {
        json_val = std::forward<X>(x);
      }

      template <typename Buffer>
      void bind_buffer(Buffer const& b)
      {
        nbdl::string temp = nbdl::util::base64_encode(b);
        bind_string(temp);
      }

      template <typename Container>
      void bind_container(Container const& c)
      {
        for (auto const& x : c)
        {
          Json::Value el;
          BindFn{}(writer(el), x);
          json_val.append(el);
        }
      }

      template<class String>
      void bind_string(String const& x)
      {
        json_val = Json::Value{&(*x.begin()), &(*x.end())};
      }

      template<typename T>
      void bind_named_member(char const* name, T&& field)
      { 
        BindFn{}(writer(json_val[name]), std::forward<T>(field));
      }

      void bind_sequence()
      { }

      template<typename X>
      void bind_sequence(X&& x)
      {
        BindFn{}(*this, std::forward<X>(x));
      }

      template<typename X1, typename X2, typename... Xs>
      void bind_sequence(X1&& x1, X2&& x2, Xs&&... xs)
      {
        hana::for_each(
          hana::make_tuple(
            std::ref(x1),
            std::ref(x2),
            std::ref(xs)...),
          [&](auto&& x) {
            Json::Value el;
            BindFn{}(writer(el), x.get());
            json_val.append(el);
          });
      }
    };

    // binds as map when possible
    struct bind_jsoncpp_fn
    {
      template <typename Binder, typename T>
      constexpr auto operator()(Binder&& binder, T&& t) const
        -> std::enable_if_t<nbdl::BindableMap<T>::value>
      {
        nbdl::bind_map(std::forward<T>(t), [&](auto&& ...pair)
        {
          (
            binder.bind_named_member(
              hana::first(pair).c_str()
            , hana::second(pair).get()
            )
          , ...
          );
        });
      }

      template <typename Binder, typename T>
      constexpr auto operator()(Binder&& binder, T&& t) const
        -> std::enable_if_t<nbdl::BindableVariant<T>::value>
      {
        hana::overload_linearly(
          [](auto&& binder, auto&& t)
            -> decltype((binder.get_variant_type_id(), void()))
          {
            nbdl::bind_variant(binder.get_variant_type_id(), std::forward<T>(t), [&](auto&& ...x)
            {
              binder.bind_sequence(std::forward<decltype(x)>(x)...);
            });
          },
          [](auto&& binder, auto&& t)
          {
            nbdl::bind_variant(std::forward<T>(t), [&](auto&& ...x)
            {
              binder.bind_sequence(std::forward<decltype(x)>(x)...);
            });
          }
        )(std::forward<Binder>(binder), std::forward<T>(t));
      }

      template <typename Binder, typename T>
      constexpr auto operator()(Binder&& binder, T&& t) const
        -> std::enable_if_t<
              nbdl::BindableSequence<T>::value
          && !nbdl::BindableMap<T>::value
        >
      {
        nbdl::bind_sequence(std::forward<T>(t), [&](auto&& ...x)
        {
          binder.bind_sequence(std::forward<decltype(x)>(x)...);
        });
      }

      template <typename Binder, typename T>
      constexpr auto operator()(Binder&& binder, T&& t) const
        -> std::enable_if_t<
             !nbdl::BindableSequence<T>::value
          && !nbdl::BindableMap<T>::value
          && !nbdl::BindableVariant<T>::value
        >
      {
        if constexpr(nbdl::String<T>::value)
        {
          binder.bind_string(std::forward<T>(t));
        }
        else if constexpr(decltype(hana::type_c<std::vector<unsigned char>> == hana::typeid_(t))::value)
        {
          binder.bind_buffer(std::forward<T>(t));
        }
        else if constexpr(nbdl::Container<T>::value)
        {
          binder.bind_container(std::forward<T>(t));
        }
        else
        {
          binder.bind_member(std::forward<T>(t));
        }
      }
    };

    constexpr bind_jsoncpp_fn bind_jsoncpp{};
  } // detail

  // `to_string` and `from_string` are just
  // temporary until functions for reading
  // and writing data are defined

  struct from_string_fn
  {
    template<typename T>
    void operator()(std::string const& json, T&& t) const
    {
      Json::Reader reader;
      Json::Value root;
      if (!reader.parse(json, root, false))
        throw std::runtime_error("JSON parse error");
      detail::reader<detail::bind_jsoncpp_fn> r(root);
      detail::bind_jsoncpp(r, std::forward<T>(t));
    }
  };

  constexpr from_string_fn from_string{};

  struct to_string_fn
  {
    template<typename T>
    std::string operator()(T&& t) const
    {
      Json::StyledWriter writer;
      Json::Value root;
      detail::writer<detail::bind_jsoncpp_fn> r(root);

      detail::bind_jsoncpp(r, std::forward<T>(t));
      return writer.write(root);
    }
  };

  constexpr to_string_fn to_string{};
}}}

#endif
