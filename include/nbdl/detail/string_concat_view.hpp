//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DETAIL_STRING_CONCAT_VIEW_HPP
#define NBDL_DETAIL_STRING_CONCAT_VIEW_HPP

#include <nbdl/concept/String.hpp>

#include <array>
#include <boost/hana/ext/std/array.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/string.hpp>
#include <dyno.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace nbdl::detail
{
  namespace hana = boost::hana;
  using std::string_view;

  // Keeping stuff in the context so I don't have to write my
  // own iterator which would probably result in more code
  constexpr auto copy_s   = BOOST_HANA_STRING("copy");
  constexpr auto equal_s  = BOOST_HANA_STRING("equal");
  constexpr auto length_s = BOOST_HANA_STRING("length");

  // TODO the interface for "equal" needs a second look :/
  struct StringConcatView : decltype(dyno::requires_(
    dyno::CopyConstructible{}
  , hana::make_pair(copy_s,   dyno::function<void(dyno::T const&, std::string::iterator)>)
  , hana::make_pair(equal_s,  dyno::function<bool(dyno::T const&, std::string::const_iterator)>)
  , hana::make_pair(length_s, dyno::function<std::size_t(dyno::T const&)>)
  )) { };
}

template <typename T>
constexpr auto dyno::default_concept_map<nbdl::detail::StringConcatView, T> = dyno::make_concept_map(
  boost::hana::make_pair(
    nbdl::detail::copy_s
  , [](T const& self, std::string::iterator itr) { self.copy(itr); }
  )
, boost::hana::make_pair(
    nbdl::detail::equal_s
  , [](T const& self, std::string::const_iterator itr) { return self.equal(itr); }
  )
, boost::hana::make_pair(
    nbdl::detail::length_s
  , [](T const& self) { return self.length(); }
  )
);

namespace nbdl::detail
{
  template <typename T>
  struct string_concat_view_impl;

  template <>
  struct string_concat_view_impl<string_view>
  {
    string_concat_view_impl(string_view x)
      : storage{std::move(x)}
    { }

    void copy(std::string::iterator itr) const
    {
      std::copy(storage.begin(), storage.end(), itr);
    }

    bool equal(std::string::const_iterator itr) const
    {
      return std::equal(storage.begin(), storage.end(), itr);
    }

    std::size_t length() const
    {
      return storage.length();
    }

  private:
    string_view storage;
  };

  // The actual dynamic type

  struct string_concat_view
  {
    string_concat_view()
      : poly_{string_concat_view_impl<string_view>{string_view{}}}
    { }

    template <typename T>
    string_concat_view(T x)
      : poly_{string_concat_view_impl<T>{std::move(x)}}
    { }

    std::string to_string() const
    {
      std::string result(length(), 0);
      copy(result.begin());
      return result;
    }

    void copy(std::string::iterator itr) const
    {
      poly_.virtual_(copy_s)(poly_, itr);
    }

    bool equal(std::string::const_iterator itr) const
    {
      return poly_.virtual_(equal_s)(poly_, itr);
    }

    std::size_t length() const
    {
      return poly_.virtual_(length_s)(poly_);
    }

  private:
    dyno::poly<StringConcatView> poly_;
  };

  // Impl for arrays of string_concat_view

  template <std::size_t size>
  struct string_concat_view_impl<std::array<string_concat_view, size>>
  {
    string_concat_view_impl(std::array<string_concat_view, size> x)
      : storage{std::move(x)}
    { }

    void copy(std::string::iterator itr) const
    {
      std::string::iterator current = itr;
      hana::for_each(storage, [&](auto const& view)
      {
        view.copy(current);
        current += view.length();
      });
    }

    bool equal(std::string::const_iterator itr) const
    {
      std::string::const_iterator current = itr;
      bool result = true;
      hana::for_each(storage, [&](auto const& view)
      {
        // TODO Consider using nbdl::promise once compile times are under control
        if (result)
        {
          result = result && view.equal(current);
          current += view.length();
        }
      });
      return result;
    }

    std::size_t length() const
    {
      return hana::unpack(storage, [](auto const& ...x)
      {
        return (x.length() + ... + 0);
      });
    }

  private:
    std::array<string_concat_view, size> storage;
  };

  // to_string_concat_view

  struct to_string_concat_view_fn
  {
    string_concat_view const& operator()(string_concat_view const& view) const
    {
      return view;
    }

    string_concat_view operator()(std::string const& str) const
    {
      return string_view(str);
    }

    string_concat_view operator()(std::reference_wrapper<std::string const> str) const
    {
      return string_view(std::cref(str).get());
    }

    template <char ...c>
    string_concat_view operator()(hana::string<c...>) const
    {
      return string_view(hana::string<c...>::c_str(), sizeof...(c));
    }

    template <String String>
    string_concat_view operator()(String const& str) const
    {
      return string_view(str.data(), str.size());
    }

    template <typename String>
    string_concat_view operator()(String const& str) const
    {
      return string_view(hana::to<char const*>(std::cref(str).get()));
    }
  };

  constexpr to_string_concat_view_fn to_string_concat_view{};

  constexpr auto make_string_concat_view = [](auto const& ...strings)
    -> string_concat_view
  {
    if constexpr(sizeof...(strings) == 1)
    {
      return to_string_concat_view(strings...);
    }
    else
    {
      return std::array<string_concat_view, sizeof...(strings)>{{
        to_string_concat_view(strings)...
      }};
    }
  };
}

#endif
